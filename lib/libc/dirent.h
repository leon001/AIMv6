
#ifndef _DIRENT_H
#define _DIRENT_H

#define DT_UNKNOWN	0
#define DT_FIFO		1
#define DT_CHR		2
#define DT_DIR		4
#define DT_BLK		6
#define DT_REG		8
#define DT_LNK		10
#define DT_SOCK		12

#define IFTODT(mode)	(((mode) & 0170000) >> 12)
#define DTTOIF(dirtype)	((dirtype) << 12)

#endif
