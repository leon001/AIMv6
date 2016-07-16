
#include <panic.h>
#include <fs/mount.h>
#include <fs/vnode.h>
#include <fs/vfs.h>

#include <fs/ufs/inode.h>
#include <fs/ufs/ufsmount.h>
#include <percpu.h>
#include <fs/namei.h>

void
fsinit(void)
{
	struct mount *rootmp;

	kprintf("KERN: Starting FS...\n");
	mountroot();
	/* get root vnode */
	rootmp = list_first_entry(&mountlist, struct mount, node);
	kpdebug("rootmp at %p\n", rootmp);
	if (VFS_ROOT(rootmp, &rootvnode) != 0)
		panic("root vnode not found\n");
	vunlock(rootvnode);

#if 0
	struct nameidata nd;
	struct vnode *vp;
	nd.path = "/";
	nd.intent = NAMEI_LOOKUP;
	nd.flags = NAMEI_FOLLOW;
	assert(namei(&nd, current_proc) == 0);
	assert(VTOI(nd.vp)->ino == ROOTINO);
	assert(VOP_CREATE(nd.vp, "a.txt", 0100777, &vp) == 0);
	vput(nd.vp);
	vput(vp);
#endif

	kprintf("==============fs test succeeded===============\n");
}

