/* Copyright (C) 2016 David Gao <davidgao1001@gmail.com>
 *
 * This file is part of AIMv6.
 *
 * AIMv6 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * AIMv6 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _FILE_H
#define _FILE_H

#include <sys/types.h>

/* FIXME */
typedef signed long loff_t;

struct inode {
	/* TODO */
	int dummy;
};

struct file {
	loff_t pos;
	struct inode * inode;
	struct file_ops * file_ops;
};

/* All devices are files, all files accept file_ops */
struct file_ops {
	loff_t (*llseek)(struct file *, loff_t, int);
	ssize_t (*read)(struct file *, char *, size_t, loff_t *);
	ssize_t (*write)(struct file *, const char *, size_t, loff_t *);
	//int (*readdir)(struct file *, void *, filldir_t);
	//unsigned int (*poll)(struct file *, struct poll_table_struct *);
	int (*ioctl)(struct file *, unsigned int, unsigned long);
	//int (*mmap)(struct file *, struct vm_area_struct *);
	int (*open)(struct inode *, struct file *);
	//int (*flush)(struct file *);
	int (*release)(struct inode *, struct file *);
	//int (*fsync)(struct file *, struct dentry *, int datasync);
	//int (*fasync)(int, struct file *, int);
	//int (*lock)(struct file *, int, struct file_lock *);
	//ssize_t (*readv)(struct file *, const struct iovec *, unsigned long,
	//	loff_t *);
	//ssize_t (*writev)(struct file *, const struct iovec *, unsigned long,
	//	loff_t *);
};

/* Only block files accept block_ops */
struct blk_ops {
	ssize_t (*readblk)(struct file *, char *, size_t, loff_t *);
	ssize_t (*writeblk)(struct file *, const char *, size_t, loff_t *);
};

#endif /* _FILE_H */

