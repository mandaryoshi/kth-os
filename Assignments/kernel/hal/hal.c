#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dr Chandra");
MODULE_DESCRIPTION("Heuristically programmed ALgorithmic computer");

static int __init hal_init(void)
{
 here:
  printk(KERN_INFO "I honestly think you ought to calm down (I'm here 0x%p);\n", &&here);
  return 0;
}

static void __exit hal_cleanup(void)
{
  printk(KERN_INFO "What are you doing, Dave?\n");
}

module_init(hal_init);
module_exit(hal_cleanup);
