/* Copyright (C) 2016 Gan Quan <coin2028@hotmail.com>
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

#include <sys/types.h>
#include <syscall.h>
#include <limits.h>
#include <fs/vnode.h>
#include <fs/namei.h>
#include <fs/uio.h>
#include <libc/syscalls.h>
#include <libc/string.h>
#include <errno.h>
#include <elfops.h>
#include <ucred.h>
#include <proc.h>
#include <percpu.h>
#include <mm.h>
#include <vmm.h>
#include <panic.h>

static int
__load_elf(struct vnode *vnode, struct elfhdr *eh, struct elf_phdr **pharr,
    struct elf_shdr **sharr, int word_len)
{
	struct elf32hdr eh32;
	struct elf32_phdr ph32;
	struct elf32_shdr sh32;
	struct elf64hdr eh64;
	struct elf64_phdr ph64;
	struct elf64_shdr sh64;
	struct elf_phdr *pharray;
	struct elf_shdr *sharray;
	void *tmpeh, *tmpph, *tmpsh;
	size_t szeh, szph, szsh;
	int err, i;
	off_t off;

	tmpeh = (word_len == 64) ? (void *)&eh64 : (void *)&eh32;
	tmpph = (word_len == 64) ? (void *)&ph64 : (void *)&ph32;
	tmpsh = (word_len == 64) ? (void *)&sh64 : (void *)&sh32;
	szeh = (word_len == 64) ? sizeof(eh64) : sizeof(eh32);
	szph = (word_len == 64) ? sizeof(ph64) : sizeof(ph32);
	szsh = (word_len == 64) ? sizeof(sh64) : sizeof(sh32);

	err = vn_read(vnode, 0, szeh, tmpeh, 0, UIO_KERNEL, current_proc, NULL,
	    NOCRED);
	if (err)
		return err;
	if (word_len == 64)
		LOAD_ELFHDR(eh, &eh64);
	else
		LOAD_ELFHDR(eh, &eh32);

	pharray = kcalloc(sizeof(*pharray), eh->phnum, 0);
	off = eh->phoff;
	for (i = 0; i < eh->phnum; ++i) {
		err = vn_read(vnode, off, szph, tmpph, 0, UIO_KERNEL,
		    current_proc, NULL, NOCRED);
		if (err)
			goto rollback_pharray;
		if (word_len == 64)
			LOAD_ELFPHDR(&pharray[i], &ph64);
		else
			LOAD_ELFPHDR(&pharray[i], &ph32);
		off += eh->phentsize;
	}

	sharray = kcalloc(sizeof(*sharray), eh->shnum, 0);
	off = eh->shoff;
	for (i = 0; i < eh->shnum; ++i) {
		err = vn_read(vnode, off, szsh, tmpsh, 0, UIO_KERNEL,
		    current_proc, NULL, NOCRED);
		if (err)
			goto rollback_sharray;
		if (word_len == 64)
			LOAD_ELFSHDR(&sharray[i], &sh64);
		else
			LOAD_ELFSHDR(&sharray[i], &sh32);
		off += eh->shentsize;
	}

	*pharr = pharray;
	*sharr = sharray;
	return 0;

rollback_sharray:
	kfree(sharray);
	*sharr = NULL;
rollback_pharray:
	kfree(pharray);
	*pharr = NULL;
	return err;
}

static int
load_elf(struct vnode *vnode, struct elfhdr *eh, struct elf_phdr **pharr,
    struct elf_shdr **sharr)
{
	char ident[EI_NIDENT];
	int err;

	/* TODO: check permissions */

	err = vn_read(vnode, 0, EI_NIDENT, ident, 0, UIO_KERNEL, current_proc,
	    NULL, NOCRED);
	if (err)
		return err;

	switch (ident[EI_CLASS]) {
	case ELFCLASS32:
		return __load_elf(vnode, eh, pharr, sharr, 32);
	case ELFCLASS64:
		return __load_elf(vnode, eh, pharr, sharr, 64);
	}
	return -ENOEXEC;
}

int
sys_execve(int sysno, int *errno, const char *ufilename, const char *argv[],
    const char *uenvp[])
{
	struct nameidata nd;
	struct mm *new_mm, *old_mm;
	struct elfhdr eh;
	struct elf_shdr *sh;
	struct elf_phdr *ph;
	char *filename;
	int err;

	if (strnlen(ufilename, PATH_MAX) == PATH_MAX) {
		*errno = ENAMETOOLONG;
		return -1;
	}

	if ((filename = kmalloc(PATH_MAX, 0)) == NULL) {
		*errno = ENOMEM;
		return -1;
	}
	strlcpy(filename, ufilename, PATH_MAX);
	kpdebug("Done copying ufilename\n");

	if ((new_mm = mm_new()) == NULL) {
		err = -ENOMEM;
		goto rollback_malloc;
	}
	kpdebug("Done mm_new()\n");

	nd.path = filename;
	nd.intent = NAMEI_LOOKUP;
	nd.flags = NAMEI_FOLLOW;

	if ((err = namei(&nd, current_proc)) != 0)
		goto rollback_mm;

	if ((err = load_elf(nd.vp, &eh, &ph, &sh)) != 0)
		goto rollback_vp;

	panic("execve done loading elf\n");

rollback_vp:
	vput(nd.vp);
rollback_mm:
	mm_destroy(new_mm);
rollback_malloc:
	kfree(filename);
	*errno = -err;
	return -1;
}
ADD_SYSCALL(sys_execve, NRSYS_execve);

