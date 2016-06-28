
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <fs/vnode.h>
#include <fs/specdev.h>
#include <fs/bio.h>
#include <buf.h>
#include <panic.h>
#include <mach-conf.h>
#include <libc/string.h>

extern struct vnode *rootvp;

void
fs_test(void)
{
	struct buf *bp;
	int err;
	char buf1[1024], buf2[1024];

	assert((err = ext2fs_mountroot()) == 0);
	assert(rootvp != NULL);
	assert(rootvp->type == VBLK);
	assert(rootvp->specinfo != NULL);
	assert(major(rootvp->specinfo->devno) == MSIM_DISK_MAJOR);
	assert(minor(rootvp->specinfo->devno) == ROOT_PARTITION_ID);
	assert(rootvp->specinfo->vnode == rootvp);
	kprintf("rootdev lock 0x%p\n", &(rootvp->lock));
	assert((err = bread(rootvp, 2, 2, &bp)) == 0);
	memcpy(buf1, bp->data, 1024);
	brelse(bp);
	assert((err = bread(rootvp, 2, 2, &bp)) == 0);
	memcpy(buf2, bp->data, 1024);
	brelse(bp);
	assert(memcmp(buf1, buf2, 1024) == 0);
	kprintf("==============fs test succeeded===============\n");
}

