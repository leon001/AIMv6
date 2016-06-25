
#include <console.h>
#include <proc.h>
#include <sched.h>
#include <libc/unistd.h>
#include <mp.h>

#include <fs/vnode.h>

extern struct vnode *rootvp;

/*
 * Temporary test
 */

void kthread(void *arg)
{
	volatile int j, cnt = 0;	/* Avoid optimizing busy loop */
	int id = (int)arg;
	kprintf("KTHREAD%d: congratulations!\n", id);
	/* sleep/wakeup test, will be removed */
	kprintf("KTHREAD%d: trying to lock rootdev\n", id);
	vlock(rootvp);
	kprintf("KTHREAD%d: locked rootdev\n", id);
	for (;;) {
		/* There is no unified timer interface so I implemented
		 * suspension as busy-wait.  Change the limit if things
		 * happen too fast for you. */
		for (j = 0; j < 100000; ++j)
			/* nothing */;
		kprintf("KTHREAD%d: running on CPU %d\n", id, cpuid());
		/* panic/IPI test, will be removed */
		if (++cnt == 5) {
			kprintf("KTHREAD%d: unlocking rootdev\n", id);
			vunlock(rootvp);
			kprintf("KTHREAD%d: unlocked rootdev\n", id);
		}
		if ((id == 3) && (++cnt == 10))
			panic("-------Test succeeded-------\n");
#if 0
		schedule();
#endif
	}
}

void userinit(void)
{
	/*
	 * XXX
	 * replace the following assembly with your arch code to quickly test
	 * (1) system call
	 * (2) sched_yield
	 * (3) user space
	 * (4) scheduler
	 */
#if 1
	asm volatile (
		"	li	$2, 1;"		/* fork() */
		"	syscall;"
		"	li	$2, 1;"		/* fork() */
		"	syscall;"
		"	li	$2, 1;"		/* fork() */
		"	syscall;"
		"1:	li	$3, 100000;"
		"2:	subu	$3, 1;"
		"	bnez	$3, 2b;"
		"	li	$2, 6;"		/* getpid() */
		"	syscall;"
		"	b	1b;"
	);
#else
	for (;;)
		/* nothing */;
#endif
}

void proc_test(void)
{
	struct proc *kthreads[5];
	struct proc *uthreads[5];
	int i;

	/*
	 * DEVELOPER NOTE:
	 * Make sure you have tested cases where
	 * (1) # of runnable processes is less than # of CPUs.
	 * (2) # of runnable processes is greater than # of CPUs.
	 * (3) transitions between (1) and (2) (not tested since fork() is
	 *     unavailable yet)
	 */
	for (i = 0; i < 5; ++i) {
		kthreads[i] = proc_new(NULL);
		proc_ksetup(kthreads[i], kthread, (void *)i);
		kthreads[i]->state = PS_RUNNABLE;
		proc_add(kthreads[i]);
	}
	for (i = 0; i < 1; ++i) {
		uthreads[i] = proc_new(NULL);
		uthreads[i]->mm = mm_new();
		assert(uthreads[i]->mm != NULL);
		assert(create_uvm(uthreads[i]->mm, (void *)PAGE_SIZE, PAGE_SIZE, VMA_READ | VMA_EXEC) == 0);
		assert(create_uvm(uthreads[i]->mm, (void *)(3 * PAGE_SIZE), PAGE_SIZE, VMA_READ | VMA_WRITE) == 0);
		assert(copy_to_uvm(uthreads[i]->mm, (void *)PAGE_SIZE, userinit, PAGE_SIZE) == 0);

		proc_usetup(uthreads[i], (void *)PAGE_SIZE, (void *)(4 * PAGE_SIZE - 32), NULL);
		uthreads[i]->state = PS_RUNNABLE;
		proc_add(uthreads[i]);
	}
}

