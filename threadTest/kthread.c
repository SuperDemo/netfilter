#include <linux/init.h>   

#include <linux/module.h>   

#include <linux/kthread.h>   

#include <linux/wait.h>

  
MODULE_LICENSE("Dual BSD/GPL");  

  
static struct task_struct * _tsk;  

static struct task_struct * _tsk1;

static int tc = 0;

static wait_queue_head_t log_wait_queue;

  

static int thread_function(void *data)  
{  

    do {  
          printk("IN thread_function thread_function: %d times \n",tc);

        
                   wait_event_interruptible(log_wait_queue,tc == 10);

                   tc = 0;  ///必须加这一行，内核才会进行调度。内核线程不像应用程序会主动调度，我们需要显式的使用调度函数，想要在thread_function_1中去重置tc的值是不可能的，因为线程不会被调度，该线程会一直占用CPU

                           
                   printk("has been woke up !\n");

    }while(!kthread_should_stop());  

    return tc;  
}  


static int thread_function_1(void *data)  
{  

    do {  printk("IN thread_function_1 thread_function: %d times\n", ++tc); 

       

                   if(tc == 10 && waitqueue_active(&log_wait_queue))

                   {

                            wake_up_interruptible(&log_wait_queue);

                   }

                   msleep_interruptible(1000);

                  
    }while(!kthread_should_stop());  

    return tc;  

}  

  
static int hello_init(void)  

{  

    printk("Hello, world!\n");  

    init_waitqueue_head(&log_wait_queue);

    _tsk = kthread_run(thread_function, NULL, "mythread"); 

    if (IS_ERR(_tsk)) {  //需要使用IS_ERR()来判断线程是否有效，后面会有文章介绍IS_ERR()

        printk("first create kthread failed!\n");  

    }  

    else {  

        printk("first create ktrhead ok!\n");  

    }  

          _tsk1 = kthread_run(thread_function_1,NULL, "mythread2");

    if (IS_ERR(_tsk1)) {  

        printk("second create kthread failed!\n");  

    }  

    else {  

        printk("second create ktrhead ok!\n");  

    }  

    return 0;  

}  

  
static void hello_exit(void)  
{  

    printk("Hello, exit!\n");  

    if (!IS_ERR(_tsk)){  

        int ret = kthread_stop(_tsk);  

        printk("First thread function has stopped ,return %d\n", ret);  

    }  

    if(!IS_ERR(_tsk1))

         {

                   int ret = kthread_stop(_tsk1);

                   printk("Second thread function_1 has stopped ,return %d\n",ret);

         }

}  

  
module_init(hello_init);  

module_exit(hello_exit);