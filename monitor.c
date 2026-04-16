#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched/signal.h>

static int __init monitor_init(void) {
    struct task_struct *task;

    printk(KERN_INFO "Monitor loaded\n");

    for_each_process(task) {

        // Ignore system processes (optional safety)
        if (task->pid > 1000) {

            // -------- Soft Limit (Warning) --------
            printk(KERN_INFO "Warning: PID %d checked\n", task->pid);

            // -------- Hard Limit (Kill one process) --------
            if (task->pid % 2 == 0) {
                printk(KERN_INFO "Killing PID %d\n", task->pid);
                send_sig(SIGKILL, task, 0);
                break;  // kill only one process
            }
        }
    }

    return 0;
}

static void __exit monitor_exit(void) {
    printk(KERN_INFO "Monitor removed\n");
}

module_init(monitor_init);
module_exit(monitor_exit);

MODULE_LICENSE("GPL");