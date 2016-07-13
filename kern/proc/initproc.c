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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <proc.h>
#include <sched.h>
#include <panic.h>
#include <libc/unistd.h>
#include <fs/vfs.h>	/* fsinit() */
#include <fs/namei.h>
#include <fs/VOP.h>
#include <fs/vnode.h>
#include <percpu.h>
#include <fcntl.h>
#include <ucred.h>

static struct proc *initproc;

char *initargv[] = {
	"/sbin/init",
	NULL
};

char *initenvp[] = {
	NULL
};

static void ttyinit(void)
{
	struct nameidata nd;

	nd.path = "/dev/tty";
	nd.intent = NAMEI_LOOKUP;
	nd.flags = NAMEI_FOLLOW;
	if (namei(&nd, current_proc) != 0)
		panic("/dev/tty: not found\n");
	if (nd.vp->type != VCHR)
		panic("/dev/tty: bad file type\n");
	assert(VOP_OPEN(nd.vp, FREAD | FWRITE, NOCRED, current_proc) == 0);

	/* Set up initproc stdin, stdout, stderr */
	current_proc->fstdin.vnode = current_proc->fstdout.vnode =
	    current_proc->fstderr.vnode = nd.vp;
}

void initproc_entry(void)
{
	/*
	 * initproc mounts the root filesystem first in kernel space,
	 * then finds the /sbin/init executable, loads it, and performs
	 * an execve(2) system call to jump into user space.
	 *
	 * The reason we mount a file system in a (kernel) process is that
	 * interacting with disks involves sleep() and wakeup(), which
	 * requires a working scheduler and interrupts enabled.
	 */
	fsinit();
	kpdebug("FS initialized\n");
	ttyinit();
	kpdebug("TTY initialized\n");

	current_proc->cwd = rootvnode;
	current_proc->rootd = rootvnode;

	execve("/sbin/init", initargv, initenvp);

	for (;;)
		/* nothing */;
}

void spawn_initproc(void)
{
	initproc = proc_new(NULL);
	proc_ksetup(initproc, initproc_entry, NULL);
	initproc->state = PS_RUNNABLE;
	initproc->groupleader = initproc;
	initproc->sessionleader = initproc;
	initproc->mainthread = initproc;
	proc_add(initproc);
}

