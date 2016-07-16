
#include <sys/types.h>
#include <fs/namei.h>
#include <fs/vnode.h>
#include <proc.h>
#include <limits.h>
#include <errno.h>
#include <vmm.h>
#include <panic.h>
#include <libc/string.h>

/*
 * Translates a path into a vnode.
 * This will be a HUUUUUGE subroutine.
 */
int
namei(struct nameidata *nd, struct proc *p)
{
	char *path_lookup = nd->path;
	char *path, *segend, *segstart;
	struct vnode *startdir;
	struct vnode *rootdir;
	struct vnode *vp;
	size_t pathlen = strnlen(path_lookup, PATH_MAX);
	int pathend, err;

	if (nd->intent != NAMEI_LOOKUP) {
		kprintf("KERN: non-lookup namei NYI\n");
		return -ENOTSUP;
	}

	if (pathlen >= PATH_MAX)
		return -ENAMETOOLONG;
	else if (pathlen == 0)
		return -ENOENT;

	assert(p != NULL);
	rootdir = p->rootd;
	if (rootdir == NULL)
		rootdir = rootvnode;
	startdir = (path_lookup[0] == '/') ? rootdir : p->cwd;
	path = kmalloc(PATH_MAX, 0);
	strlcpy(path, path_lookup, PATH_MAX);

	/*
	 * Here we are dealing with the simplest case: the file we are
	 * looking up, as well as all the directories we are tracing
	 * down, are all on the same file system.  On Unix-like systems,
	 * things are much more complicated.
	 */
	vget(startdir);
	vp = rootdir;		/* If we found a '/', return the rootdir */
	err = 0;
	for (segstart = path; *segstart != '\0'; segstart = segend + 1) {
		/* chomp a segment */
		for (segend = segstart; *segend != '/' && *segend != '\0';
		    ++segend)
			/* nothing */;
		pathend = (*segend == '\0');
		*segend = '\0';
		if (segstart == segend)
			continue;

		/*
		 * Usually, the file system provider should deal with
		 * special segments "." and "..".  There are cases
		 * (e.g. ext2) where the on-disk directories store
		 * the file ID of parent directories.
		 *
		 * However, if we arrive at the root vnode of a process
		 * (due to chroot(2)), or the root vnode of the whole
		 * file system tree, we will have to do a NOP by our own.
		 *
		 * Also, there are cases where we are crossing file systems
		 * if current directory is the root of a file system mounted
		 * on a directory of another file system.  We do not support
		 * mounting multiple file systems right now.
		 */
		if (strcmp(segstart, "..") == 0) {
			if (startdir == rootdir || startdir == rootvnode)
				continue;
		}

		/* look for that segment in the directory */
		err = VOP_LOOKUP(startdir, segstart, &vp);
		vput(startdir);
		if (err) {
			nd->vp = NULL;
			goto finish;
		}

		/* symlink? */
		if ((nd->flags & NAMEI_FOLLOW) && vp->type == VLNK) {
			/* TODO */
			kprintf("KERN: symlink lookup NYI\n");
			nd->vp = NULL;
			vput(vp);
			err = -ENOTSUP;
			goto finish;
		}
		/* final segment? */
		if (pathend) {
			nd->vp = vp;
			goto finish;
		}
		/* is it a directory?  for further lookups */
		if (vp->type == VDIR) {
			/* Note that vp is already vget'd in VOP_LOOKUP;
			 * we don't need to vget() it again. */
			startdir = vp;
		} else {
			/* trying to get directory entry inside a file...
			 * put the vnode we just vget'd back */
			assert(vp != NULL);
			assert(vp->type != VDIR);
			assert(vp->type != VNON && vp->type != VBAD);
			nd->vp = NULL;
			vput(vp);
			err = -ENOTDIR;
			goto finish;
		}
	}
	nd->vp = vp;

finish:
	kfree(path);
	return err;
}

