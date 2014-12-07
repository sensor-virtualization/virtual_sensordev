#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>

#define STRING_SIZE 	64
static char sdata[STRING_SIZE] = "0 0 0";
static DEFINE_MUTEX(llock);

static int vsensor_open(struct inode *inode, struct file *file)
{
    printk("I have been awoken\n");
    return 0;
}
 
static int vsensor_close(struct inode *inodep, struct file *filp)
{
    printk("Sleepy time\n");
    return 0;
}
 
static ssize_t vsensor_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    int error = 0;

    printk("Vsensor Write : len is %d bytes\n",len);

    if (len >= STRING_SIZE)
    {
        printk("Vsensor Write Error : len is %d bytes, bigger than %d\n",len, STRING_SIZE);
        return -EINVAL; 
    }

    /*copy user data*/
    mutex_lock(&llock);
    sdata[0]='\0';
    error = copy_from_user(sdata, buf, len);
    mutex_unlock(&llock);
    printk("Vsensor Write : data string is %s \n",sdata);
    if (error)
    {
        printk("Vsensor Error : copy_from_user error >> %d\n", error);
        return -EINVAL;
    }
    
    return len;
}

static ssize_t vsensor_read(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    int error = 0;
    int length = strlen(sdata);
    
    if (len < length) /* Allow all string read at once */
        return -EINVAL;

    if (*ppos != 0) /* Allow one read at once */
        return 0;

    printk("Vsensor Read : len is %d bytes\n",len);
    /* copy sensor data to user buffer */
    mutex_lock(&llock);
    error = copy_to_user((char *)buf, sdata, length);
    //memcpy((char *)buf, sdata, 64);
    mutex_unlock(&llock);
    printk("Vsensor Read : data string is %s \n",buf);
    if (error)
    {
        printk("Vsensor Error : copy_to_user error >> %d\n", error);
        return -EINVAL;
    }
    *ppos = length; /*Inform progress to user*/
    return length; /* But we don't actually do anything with the data */
}

static const struct file_operations vsensor_fops = {
    .owner			= THIS_MODULE,
    .read			= vsensor_read,
    .write			= vsensor_write,
    .open			= vsensor_open,
    .release			= vsensor_close,
};
 
struct miscdevice vsensor_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "vsensor",
    .fops = &vsensor_fops,
};
 
static int __init vsensor_init(void)
{
    int error;
 
    error = misc_register(&vsensor_device);
    if (error) {
        printk("can't misc_register \n");
        return error;
    }
    
    mutex_init(&llock); 
    printk("I'm in\n");
    return 0;
}
 
static void __exit vsensor_exit(void)
{
    misc_deregister(&vsensor_device);
    printk("I'm out\n");
}
 
module_init(vsensor_init)
module_exit(vsensor_exit)
 
MODULE_DESCRIPTION("Simple Misc Driver");
MODULE_AUTHOR("Nick Glynn <n.s.glynn@gmail.com>");
MODULE_LICENSE("GPL");
