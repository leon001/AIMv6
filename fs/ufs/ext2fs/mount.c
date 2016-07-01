
#include <fs/vnode.h>
#include <fs/specdev.h>
#include <fs/VOP.h>
#include <fs/mount.h>
#include <fs/vfs.h>
#include <fs/bio.h>
#include <fs/ufs/ext2fs/ext2fs.h>
#include <fs/ufs/ext2fs/dinode.h>
#include <aim/initcalls.h>
#include <panic.h>
#include <ucred.h>
#include <fcntl.h>
#include <proc.h>
#include <percpu.h>
#include <mach-conf.h>
#include <libc/string.h>
#include <limits.h>
#include <errno.h>
#include <bitmap.h>

extern dev_t rootdev;	/* initialized in mach_init() or arch_init() */
extern struct vnode *rootvp;

static struct vfsops ext2fs_vfsops = {
	0
};

int ext2fs_mountfs(struct vnode *, struct mount *, struct proc *);

int
ext2fs_mountroot(void)
{
	int err;
	struct mount *mp;

	if (bdevvp(rootdev, &rootvp))
		panic("ext2fs_mountroot: can't setup bdevvp's\n");

	if ((err = vfs_rootmountalloc("ext2fs", &mp)) != 0) {
		kpdebug("rolling back rootvp\n");
		goto rollback_rootvp;
	}

	if ((err = ext2fs_mountfs(rootvp, mp, current_proc)) != 0) {
		kpdebug("rolling back mountalloc\n");
		goto rollback_mountalloc;
	}

	return 0;

rollback_mountalloc:
	vfs_mountfree(&mp);
rollback_rootvp:
	vrele(rootvp);
	return err;
}

static void
__fs_test(void)
{
	struct buf *bp;
	int err;
	char buf1[1024], buf2[1024];

	assert(rootvp != NULL);
	assert(rootvp->type == VBLK);
	assert(rootvp->specinfo != NULL);
	assert(major(rootvp->specinfo->devno) == MSIM_DISK_MAJOR);
	assert(minor(rootvp->specinfo->devno) == ROOT_PARTITION_ID);
	assert(rootvp->specinfo->vnode == rootvp);
	assert((err = bread(rootvp, 3, 2, &bp)) == 0);
	memcpy(buf1, bp->data, 1024);
	brelse(bp);
	assert((err = bread(rootvp, 3, 2, &bp)) == 0);
	memcpy(buf2, bp->data, 1024);
	brelse(bp);
	assert(memcmp(buf1, buf2, 1024) == 0);
}

/* temporary, will be removed in release */
void
__ext2fs_dump(struct vnode *devvp, struct m_ext2fs *fs)
{
	int i, err, n_zero, n_one;
	struct buf *bp;
	/*
	 * Please, do check the following dumps with the ones from dumpe2fs(8).
	 */
	kprintf("ext2 inode count: %d\n", fs->e2fs.icount);
	kprintf("ext2 block count: %d\n", fs->e2fs.bcount);
	kprintf("ext2 reserved blocks count: %d\n", fs->e2fs.rbcount);
	kprintf("ext2 free blocks count: %d\n", fs->e2fs.fbcount);
	kprintf("ext2 free inode count: %d\n", fs->e2fs.ficount);
	kprintf("ext2 first data block: %d\n", fs->e2fs.first_dblock);
	kprintf("ext2 blocks per group: %d\n", fs->e2fs.bpg);
	kprintf("ext2 frags per group: %d\n", fs->e2fs.fpg);
	kprintf("ext2 inodes per group: %d\n", fs->e2fs.ipg);
	kprintf("ext2 revision: %d.%d\n", fs->e2fs.rev, fs->e2fs.minrev);
	kprintf("ext2 frag size: %d\n", fs->fsize);
	kprintf("ext2 block size: %d\n", fs->bsize);
	kprintf("ext2 # cylinder groups: %d\n", fs->ncg);
	kprintf("ext2 # group descriptor block: %d\n", fs->ngdb);
	kprintf("ext2 # inodes per block: %d\n", fs->ipb);
	kprintf("ext2 # inode tables per group: %d\n", fs->itpg);
	for (i = 0; i < fs->ncg; ++i) {
		/* XXX twisted: on ext2 with 1KB logical blocks, super blocks
		 * reside on block 1, pushing everything one block away. */
		size_t sb_off = (fs->bsize == 1024) ? 1 : 0;
		size_t block_base = fs->e2fs.bpg * i + sb_off;
		size_t inode_base = fs->e2fs.ipg * i;
		kprintf("ext2 Group #%d\n", i);
		kprintf("\tsuperblock backup at %d\n", block_base);
		kprintf("\tblock group desc at %d\n", block_base + 1);
		kprintf("\tblocks bitmap at %d\n", fs->gd[i].b_bitmap);
		kprintf("\tinode bitmap at %d\n", fs->gd[i].i_bitmap);
		kprintf("\tinode tables at %d-%d\n", fs->gd[i].i_tables,
		    fs->gd[i].i_tables + fs->itpg - 1);
		kprintf("\t# free blocks: %d\n", fs->gd[i].nbfree);
		kprintf("\t# free inodes: %d\n", fs->gd[i].nifree);
		kprintf("\t# directories: %d\n", fs->gd[i].ndirs);

		/* Process ext2fs block bitmap: 0 - available, 1 - used */
		err = bread(devvp, fsbtodb(fs, fs->gd[i].b_bitmap),
		    fs->bsize / BLOCK_SIZE, &bp);
		if (err != 0) {
			kprintf("\tbread error\n");
			brelse(bp);
			return;
		}
		/* XXX this piece of code is twisted: bitmap_find_first() and
		 * bitmap_find_next() functions are 1-based, and returns 0 when
		 * not found, but the disk block indices are 0-based. */
		n_zero = bitmap_find_first_zero_bit(bp->data, fs->e2fs.bpg);
		do {
			n_one = bitmap_find_next_bit(bp->data, fs->e2fs.bpg, n_zero);
			kprintf("\tfree block set (inclusive): %d-%d\n",
			    n_zero - 1 + block_base,
			    /* - 2 because of right inclusion */
			    n_one ? n_one - 2 + block_base : 
			    min2(block_base + fs->e2fs.bpg - 1, fs->e2fs.bcount - 1));
			n_zero = bitmap_find_next_zero_bit(bp->data, fs->e2fs.bpg, n_one);
		} while (n_zero != 0);
		brelse(bp);

		err = bread(devvp, fsbtodb(fs, fs->gd[i].i_bitmap),
		    fs->bsize / BLOCK_SIZE, &bp);
		if (err != 0) {
			kprintf("\tbread error\n");
			brelse(bp);
			return;
		}
		/* XXX note that inodes are 1-based */
		n_zero = bitmap_find_first_zero_bit(bp->data, fs->e2fs.ipg);
		do {
			n_one = bitmap_find_next_bit(bp->data, fs->e2fs.ipg, n_zero);
			kprintf("\tfree inode set (inclusive): %d-%d\n",
			    n_zero + inode_base,
			    /* - 1 because of right inclusion */
			    n_one ? n_one - 1 + inode_base :
			    min2(inode_base + fs->e2fs.ipg, fs->e2fs.icount));
			n_zero = bitmap_find_next_zero_bit(bp->data, fs->e2fs.ipg, n_one);
		} while (n_zero != 0);
		brelse(bp);
	}
}

/*
 * Check if the on-disk super block is valid.
 * Very ext2-specific, mostly comes from OpenBSD code and is not particularly
 * interesting.
 */
int
ext2fs_sbinit(struct vnode *devvp, struct ext2fs *ondiskfs, struct m_ext2fs *sb)
{
	struct ext2fs *fs = &sb->e2fs;
	int err, i;
	struct buf *bp;

	e2fs_sbload(ondiskfs, fs);

	if (fs->magic != E2FS_MAGIC ||
	    fs->log_bsize > 2 ||
	    fs->bpg == 0 ||
	    fs->rev > 1 ||
	    /*
	     * We only support ext2 rev. 0 so we do not check for extended
	     * features.  However, when Linux mounts an ext2 rev. 0 file system,
	     * it changes the revision number to 1 (which is harmless but
	     * annoying).  So we have to ensure that no rev. 1 features are
	     * turned on (which is mostly the case) with additional logic.
	     */
	    fs->features_incompat != 0 ||
	    fs->features_rocompat != 0 ||
	    fs->inode_size != EXT2_REV0_DINODE_SIZE) {
		kprintf("ext2fs invalid: %x %d %d %d %d %d %d\n", fs->magic,
		    fs->log_bsize, fs->bpg, fs->rev, fs->features_incompat,
		    fs->features_rocompat, fs->inode_size);
		return -EINVAL;
	}

	sb->ncg = DIV_ROUND_UP(fs->bcount - fs->first_dblock, fs->bpg);
	sb->fsbtodb = fs->log_bsize + 1;
	sb->bsize = MINBSIZE << fs->log_bsize;
	sb->bshift = LOG_MINBSIZE + fs->log_bsize;
	sb->fsize = MINFSIZE << fs->log_fsize;
	sb->qbmask = sb->bsize - 1;
	sb->bmask = ~sb->qbmask;
	sb->ipb = sb->bsize / EXT2_DINODE_SIZE(sb);
	sb->itpg = fs->ipg / sb->ipb;

	/* Load group descriptors */
	sb->ngdb = DIV_ROUND_UP(sb->ncg, sb->bsize / sizeof(struct ext2_gd));
	sb->gd = kcalloc(sb->ngdb, sb->bsize, 0);
	for (i = 0; i < sb->ngdb; ++i) {
		unsigned int dblk = ((sb->bsize > 1024) ? 1 : 2) + i;
		size_t gdesc = i * sb->bsize / sizeof(struct ext2_gd);
		struct ext2_gd *gd;

		err = bread(devvp, fsbtodb(sb, dblk), sb->bsize / BLOCK_SIZE, &bp);
		if (err) {
			kfree(sb->gd);
			sb->gd = NULL;
			brelse(bp);
			return err;
		}
		gd = bp->data;
		e2fs_cgload(gd, &sb->gd[gdesc], sb->bsize);
		brelse(bp);
		bp = NULL;
	}

	__ext2fs_dump(devvp, sb);

	sb->maxfilesize = INT_MAX;	/* Rev. 0 */
	return 0;
}

int
ext2fs_mountfs(struct vnode *devvp, struct mount *mp, struct proc *p)
{
	int err;
	struct buf *bp;
	struct ext2fs *fs;
	struct m_ext2fs *sb;

	/*
	 * TODO:
	 * Disallow multiple mounts on the same device
	 * Disallow mounting a device in use
	 */

	kpdebug("opening rootdev\n");
	err = VOP_OPEN(devvp, FREAD | FWRITE, NOCRED, p);
	if (err != 0)
		return err;

	vlock(devvp);
	kpdebug("reading ext2fs super block\n");
	err = bread(devvp, SBOFF / BLOCK_SIZE, SBSIZE / BLOCK_SIZE, &bp);
	if (err != 0)
		goto rollback_open;

	sb = kmalloc(sizeof(*sb), 0);
	fs = bp->data;
	err = ext2fs_sbinit(devvp, fs, sb);
	if (err != 0)
		goto rollback_alloc;

	brelse(bp);

	/* TODO: temporary test */
	__fs_test();

	return 0;

rollback_alloc:
	kfree(sb);
	brelse(bp);
rollback_open:
	VOP_CLOSE(devvp, FREAD | FWRITE, NOCRED, p);
	vunlock(devvp);

	return err;
}

int
ext2fs_register(void)
{
	registerfs("ext2fs", &ext2fs_vfsops);
	return 0;
}
INITCALL_FS(ext2fs_register);

