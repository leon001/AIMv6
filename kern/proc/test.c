
#include <console.h>
#include <proc.h>
#include <sched.h>

/*
 * Temporary test since I haven't implemented scheduler and timer yet.
 */

void kthread1(void)
{
	int j;
	kprintf("KTHREAD1: congratulations!\n");
	for (;;) {
		for (j = 0; j < 100000; ++j)
			/* nothing */;
		kprintf("KTHREAD1: running here\n");
		schedule();
	}
}

void kthread2(void)
{
	int j;
	kprintf("KTHREAD2: congratulations!\n");
	for (;;) {
		for (j = 0; j < 100000; ++j)
			/* nothing */;
		kprintf("KTHREAD2: running there\n");
		schedule();
	}
}

void proc_test(void)
{
	struct proc *kthread1_proc, *kthread2_proc;

	kthread1_proc = proc_new(NULL);
	kthread1_proc->mm = mm_new();
	proc_ksetup(kthread1_proc, kthread1, kthread1_proc->kstack, NULL);
	kthread1_proc->state = PS_RUNNABLE;
	proc_add(kthread1_proc);

	kthread2_proc = proc_new(NULL);
	kthread2_proc->mm = mm_new();
	proc_ksetup(kthread2_proc, kthread2, kthread2_proc->kstack, NULL);
	kthread2_proc->state = PS_RUNNABLE;
	proc_add(kthread2_proc);

	switch_context(kthread1_proc);
}

