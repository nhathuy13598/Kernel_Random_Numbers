#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>          // Required for the copy to user function
#include <linux/random.h>
#define  DEVICE_NAME "RandomMachine"    ///< The device will appear at /dev/RANDOMMACHINE using this value
#define  CLASS_NAME  "Random"        ///< The device class -- this is a character device driver

MODULE_LICENSE("GPL");

static int    majorNumber;                  ///< Stores the device number -- determined automatically
static int    numberOpens = 0;              ///< Counts the number of times the device is opened
static struct class*  randomClass  = NULL; ///< The device-driver class struct pointer
static struct device* randomDevice = NULL; ///< The device-driver device struct pointer

// The prototype functions for the character driver -- must come before the struct definition
static int randomNumber;
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);


static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .release = dev_release,
};


static int __init RANDOMMACHINE_init(void){
   printk(KERN_INFO "RANDOMMACHINE: Initializing the RANDOMMACHINE LKM\n");

   // Try to dynamically allocate a major number for the device -- more difficult but worth it
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      printk(KERN_ALERT "RANDOMMACHINE failed to register a major number\n");
      return majorNumber;
   }
   printk(KERN_INFO "RANDOMMACHINE: registered correctly with major number %d\n", majorNumber);

   // Register the device class
   randomClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(randomClass)){                // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to register device class\n");
      return PTR_ERR(randomClass);          // Correct way to return an error on a pointer
   }
   printk(KERN_INFO "RANDOMMACHINE: device class registered correctly\n");

   // Register the device driver
   randomDevice = device_create(randomClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(randomDevice)){               // Clean up if there is an error
      class_destroy(randomClass);           // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME);
      printk(KERN_ALERT "Failed to create the device\n");
      return PTR_ERR(randomDevice);
   }
   printk(KERN_INFO "RANDOMMACHINE: device class created correctly\n"); // Made it! device was initialized
   return 0;
}


static void __exit RANDOMMACHINE_exit(void){
   device_destroy(randomClass, MKDEV(majorNumber, 0));     // remove the device
   class_unregister(randomClass);                          // unregister the device class
   class_destroy(randomClass);                             // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   printk(KERN_INFO "RANDOMMACHINE: Goodbye from the LKM!\n");
}

    
static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   printk(KERN_INFO "RANDOMMACHINE: Device has been opened %d time(s)\n", numberOpens);
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   get_random_bytes(&randomNumber,sizeof(int));
   return randomNumber;
}




static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "RANDOMMACHINE: Device successfully closed\n");
   return 0;
}


module_init(RANDOMMACHINE_init);
module_exit(RANDOMMACHINE_exit);



