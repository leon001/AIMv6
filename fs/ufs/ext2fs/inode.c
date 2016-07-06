
#include <sys/types.h>
#include <fs/ufs/inode.h>
#include <fs/ufs/ext2fs/dinode.h>
#include <fs/ufs/ext2fs/ext2fs.h>
#include <errno.h>

int
ext2fs_setsize(struct inode *ip, uint64_t size)
{
	struct m_ext2fs *fs = ip->superblock;
	struct ext2fs_dinode *dp = ip->dinode;

	if (size <= fs->maxfilesize) {
		if ((dp->mode & EXT2_IFMT) == EXT2_IFREG || dp->mode == 0)
			dp->size_hi = size >> 32;
		dp->size = size;
		return 0;
	}
	return -EFBIG;
}

