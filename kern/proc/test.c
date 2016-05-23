
#include <console.h>
#include <proc.h>

/*
 * Temporary test since I haven't implemented scheduler and timer yet.
 */

void kthread1(void)
{
	kprintf("KTHREAD1: congratulations!\n");
	for (;;)
		/* nothing */;
}

void proc_test(void)
{
	struct proc *kthread1_proc;

	kthread1_proc = proc_new(NULL);
	kthread1_proc->mm = mm_new();
	proc_ksetup(kthread1_proc, kthread1, kthread1_proc->kstack, 0, NULL,
	    NULL);

	switch_context(kthread1_proc);
}
