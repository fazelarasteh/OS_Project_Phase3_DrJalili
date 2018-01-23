#include <linux/module.h>    // for all kernel modules
#include <linux/kernel.h>    // for KERN_INFO
#include <linux/init.h>      // for __init and __exit macros
#include <linux/sched.h>     // for_each_process()
#include <linux/proc_fs.h>
#include <linux/kthread.h>
#include <linux/delay.h>




MODULE_LICENSE("");       // set licence of module
MODULE_AUTHOR("");    // authors of modules
MODULE_DESCRIPTION("gam 2");    // module description
static struct task_struct *thread_reporter;
static pid_t thread_id;
static int prev[20000] = {0};

int my_mod(void *unused_data)
{
	allow_signal(SIGKILL);
	while(!kthread_should_stop()){
		struct task_struct* task;
       		for_each_process(task) {
               	        if(task->mm != NULL && task->pid < 20000){
				int diff = task->mm->total_vm - prev[task->pid];
				prev[task->pid] = task->mm->total_vm;
				if (diff > 0)                      		 
					printk(KERN_INFO "****************Allocated %d for PID = %d****************\n", diff, task->pid);
				else if (diff < 0)
					printk(KERN_INFO "****************Freed %d from PID = %d\n****************", -1*diff, task->pid);
        	        }
	    	}
		ssleep(3);
		if (signal_pending(thread_reporter)){
			printk(KERN_INFO "****************The SIGKILL has been sent to thread successfully****************");
            		break;
		}
	}
	printk(KERN_INFO "****************Thread goodbye!****************");
        return 0;
}


static int __init my_mod_init(void) {     // init function for hello_os module
    	printk(KERN_INFO "****************init****************\n");
        thread_reporter = kthread_run(&my_mod, NULL, "");

	if (thread_reporter){
		thread_id = thread_reporter->pid;
		printk(KERN_INFO "****************Thread init****************");
	}
	else {
		thread_id = 0;
		printk(KERN_INFO "****************Thread init failed****************");
	}
 	return 0;    // Non-zero return means that the module couldn't be loaded.
}

static void __exit my_mod_cleanup(void) {
	kthread_stop(thread_reporter);
	printk(KERN_INFO "****************killed****************");
}

module_init(my_mod_init);     
module_exit(my_mod_cleanup); 
