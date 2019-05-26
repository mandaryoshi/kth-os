#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros

#include <linux/proc_fs.h>   // file operations
#include <linux/seq_file.h>  // seq_read, ...

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dr. Dyson");
MODULE_DESCRIPTION("Global Information Grid");

static int skynet_show(struct seq_file *m, void *v);

static int skynet_open(struct inode *inode, struct  file *file);

static const struct file_operations skynet_fops = {
  .owner = THIS_MODULE,
  .open = skynet_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};


static int skynet_show(struct seq_file *m, void *v) {
 here:
  seq_printf(m, "Skynet location: 0x%lx\n", (unsigned long)&&here);
  return 0;
}

static int skynet_open(struct inode *inode, struct  file *file) {
  return single_open(file, skynet_show, NULL);
}


static int __init skynet_init(void) {
  proc_create("skynet", 0, NULL, &skynet_fops);
  printk(KERN_INFO "Skynet in control\n");

  return 0;
}

static void __exit skynet_cleanup(void) {
  remove_proc_entry("skynet", NULL);
  printk(KERN_INFO "I'll be back!\n");
}

module_init(skynet_init);
module_exit(skynet_cleanup);
