
#include <console.h>
#include <proc.h>
#include <sched.h>
#include <libc/unistd.h>
#include <mp.h>

/*
 * Temporary test since I haven't implemented scheduler and timer yet.
 */

void kthread(void *arg)
{
	int j;
	int id = (int)arg;
	kprintf("KTHREAD%d: congratulations!\n", id);
	for (;;) {
		/* There is no unified timer interface so I implemented
		 * suspension as busy-wait.  Change the limit if things
		 * happen too fast for you. */
		for (j = 0; j < 100000; ++j)
			/* nothing */;
		kprintf("KTHREAD%d: running on CPU %d\n", id, cpuid());
		/* TODO: create user threads instead of kernel threads.
		 * Here I'm just checking whether system call framework
		 * works. */
		sched_yield();
	}
}

void proc_test(void)
{
	struct proc *kthreads[5];
	int i;

	/*
	 * DEVELOPER NOTE:
	 * Make sure you have tested cases where
	 * (1) # of runnable processes is less than # of CPUs.
	 * (2) # of runnable processes is greater than # of CPUs.
	 * (3) transitions between (1) and (2) (not tested since fork() is
	 *     unavailable yet)
	 */
	for (i = 0; i < 1; ++i) {
		kthreads[i] = proc_new(NULL);
		kprintf("KTHREAD %d: PID %d\n", i, kthreads[i]->kpid);
		proc_ksetup(kthreads[i], kthread, (void *)i);
		kthreads[i]->state = PS_RUNNABLE;
		proc_add(kthreads[i]);
	}
}

