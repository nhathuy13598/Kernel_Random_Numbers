#include <linux/init.h>   
#include <linux/module.h> 
#include <linux/device.h> 
#include <linux/kernel.h> 
#include <linux/fs.h>     
#include <linux/random.h> ///< Can cho ham get_random_bytes
#define DEVICE_NAME "RandomMachine" 
#define CLASS_NAME "Random"         

MODULE_LICENSE("GPL");

static int majorNumber;                    
static struct class *randomClass = NULL;   
static struct device *randomDevice = NULL; 
static unsigned char randomNumber; ///< Chuoi chua cac chu so cua randomNumber
static char temp[4] = {'\0'};
static int i = 0;



static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops =
    {
        .open = dev_open,
        .read = dev_read,
        .release = dev_release,
};

static int __init RANDOMMACHINE_init(void)
{
    // Cap phat major number
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0)
    {
        printk(KERN_ALERT "RANDOMMACHINE failed to register a major number\n");
        return majorNumber;
    }
    
    // Dang ky device class
    randomClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(randomClass))
    { 
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(randomClass); 
    }
    
    // Dang ky device driver
    randomDevice = device_create(randomClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(randomDevice))
    {                              
        class_destroy(randomClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(randomDevice);
    }

    printk(KERN_INFO "RANDOMMACHINE: device class created correctly\n"); 
    return 0;
}

static void __exit RANDOMMACHINE_exit(void)
{
    device_destroy(randomClass, MKDEV(majorNumber, 0)); // huy thiet bi
    class_unregister(randomClass);                      // huy dang ky device class
    class_destroy(randomClass);                         // xoa device class
    unregister_chrdev(majorNumber, DEVICE_NAME);        // huy dang ky device major number
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "RANDOMMACHINE: Device is opening\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    i = 0;
    get_random_bytes(&randomNumber, sizeof(char));
    printk(KERN_INFO "RANDOMMACHINE: Random number is %d\n", randomNumber);
    if (len < 4)
    {
        printk(KERN_INFO "\n\nRANDOMMACHINE: Failed\n");
        return -EFAULT;
    }
    if (randomNumber != 0)
    {
	
        while (randomNumber != 0)
        {
            temp[i] = randomNumber % 10 + '0';
            randomNumber = randomNumber / 10;
            i++;
        }
        temp[i] = '\0';
        buffer[i] = '\0';
        i -= 1;
        while (i >= 0)
        {
            *buffer = temp[i];
            i -= 1;
            buffer += 1;
        }
        return 0;
    }
    else
    {

        *(buffer++) = '0';
        *buffer = '\0';
        return 0;
    }
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "RANDOMMACHINE: Device successfully closed\n");
    return 0;
}

module_init(RANDOMMACHINE_init);
module_exit(RANDOMMACHINE_exit);
