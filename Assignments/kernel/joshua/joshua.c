#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // KERN_INFO

#include <linux/fs.h>        // file_operations, ioctl ...

#include <linux/cdev.h>      // cdev_init, cdev_all  handles character devices
#include <linux/device.h>    // class_create, class_destroy, device_create, device_destroy
#include <linux/uaccess.h>     // copy_to_user, copy_from_use

#include "joshua.h"          // to agree on the interface

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prof Franken");
MODULE_DESCRIPTION("Joshua");

#define FIRST_MINOR 0
#define MINOR_CNT 1

static int joshua_open(struct inode *i, struct file *f);
static int joshua_close(struct inode *i, struct file *f);
static long joshua_ioctl(struct file *f, unsigned int cmd, unsigned long arg);

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

static struct file_operations joshua_fops = {
  .owner = THIS_MODULE,
  .open = joshua_open,
  .release = joshua_close,
  .unlocked_ioctl = joshua_ioctl
};

#define QUOTES 3

static const char * quotes[QUOTES] = {
  "Why play a game that can not be won?",
  "Mutual destruction is not a victory.",
  "Simulation is the mother of knowledge."
};

static int next = 0;


static int __init joshua_init(void) {
  int ret;
  struct device *dev_ret;

  printk(KERN_INFO "Want to play a game?\n");

  if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "joshua")) < 0) {
    return ret;
  }

  cdev_init(&c_dev, &joshua_fops);

  if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0) {
    return ret;
  }

  if (IS_ERR(cl = class_create(THIS_MODULE, "char")))  {
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    return PTR_ERR(cl);
  }
  if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "joshua"))) {
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    return PTR_ERR(dev_ret);
  }

  return 0;
}

static void __exit joshua_exit(void) {
  device_destroy(cl, dev);
  class_destroy(cl);
  cdev_del(&c_dev);
  unregister_chrdev_region(dev, MINOR_CNT);

  printk(KERN_INFO "How about a nice game of chess?\n");
}

static int joshua_open(struct inode *i, struct file *f) {
  return 0;
}

static int joshua_close(struct inode *i, struct file *f) {
  return 0;
}

static long joshua_ioctl(struct file *f, unsigned int cmd, unsigned long arg)  {

  switch (cmd)  {

    case JOSHUA_GET_QUOTE:

      next = (next+1) % QUOTES;

      printk(KERN_INFO "Joshua: copy to buffer at 0x%lx\n", arg);

      if (copy_to_user((char *)arg, quotes[next], JOSHUA_MAX))
	{
	  return -EACCES;
	}
      break;

    default:
      return -EINVAL;
    }
  return 0;
}


module_init(joshua_init);
module_exit(joshua_exit);
