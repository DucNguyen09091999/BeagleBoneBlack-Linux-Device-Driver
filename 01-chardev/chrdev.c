#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#define DRIVER_AUTHOR "DucNguyen nguyenchiduc090999@gmail.com"
#define DRIVER_DESCRIPTION "character driver kernel module"

#define CDEV_NAME "m-cdev"
#define CLASS_NAME "m-class"
#define DEVICE_NAME "m-device"
#define DEV_MEM_SIZE 512
//Pseudo device's memory

char dev_buf[DEV_MEM_SIZE];
struct m_foo_dev
{
    dev_t dev_num;
    struct cdev m_cdev;
    struct class *m_class;
}mdev;

static int __init m_driver_init(void);
static void __exit m_driver_exit(void);
static loff_t m_lseek (struct file *filp, loff_t off, int whence);
static ssize_t m_read (struct file *filp, char __user *buff, size_t count, loff_t *f_pos);
static ssize_t m_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
static int m_open (struct inode *inode, struct file *filp);
static int m_release (struct inode *inode, struct file *filp);


static struct file_operations m_fops =
{
    .owner      = THIS_MODULE,
    .read       = m_read,
    .write      = m_write,
    .open       = m_open,
    .release    = m_release,
    .llseek     = m_lseek,
};

static loff_t m_lseek (struct file *filp, loff_t off, int whence)
{
    pr_info("[Info] System call lseek() called...\n");
    return 0;
}
static ssize_t m_read (struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    pr_info("[Info] System call read() called...\n");
    return 0;
}
static ssize_t m_write (struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    pr_info("[Info] System call write() called...\n");
    return count;
}
static int m_open (struct inode *inode, struct file *filp)
{
    pr_info("[Info] System call open() called...\n");
    return 0;
}
static int m_release (struct inode *inode, struct file *filp)
{
    pr_info("[Info] System call release() called...\n");
    return 0;
}

static int
__init m_driver_init(void){

        //1.0 dynamic allocate a device number
    if((alloc_chrdev_region(&mdev.dev_num,0,1,CDEV_NAME)) < 0){
        pr_err("Can't allocate chrdev region\n");
        return -1;
    }
    pr_info("[INFO] Major = %d, Minor = %d\n",MAJOR(mdev.dev_num),MINOR(mdev.dev_num));

        //1.1 Initialize cdev structure with fops
    cdev_init(&mdev.m_cdev,&m_fops);
    
        //1.2 Register cdev structure with VFS
        //mdev.m_cdev.owner = THIS_MODULE;
    if(cdev_add(&mdev.m_cdev,mdev.dev_num,1) < 0){
        pr_err("Can't register cdev structure to the system\n");
        goto rm_dev_numb;
    }

        //1.3 Create device class under /sys/class/
    if((mdev.m_class = class_create(THIS_MODULE,CLASS_NAME)) == NULL){
        pr_err("Can't create device class for my device\n");
        goto rm_dev_numb;
    }

        //1.4 Creating device
    if((device_create(mdev.m_class,NULL,mdev.dev_num,NULL,DEVICE_NAME)) == NULL){
        pr_err("Can't create my device\n");
        goto rm_class;
    }

    printk(KERN_INFO "Hello world kernel module\n");
    return 0;

    rm_class:
        class_destroy(mdev.m_class);
    rm_dev_numb:
        unregister_chrdev_region(mdev.dev_num,1);
        return -1;

}

static void
__exit m_driver_exit(void){
    //1.3 Destroy device
    device_destroy(mdev.m_class,mdev.dev_num);
    //1.2 Destroy class
    class_destroy(mdev.m_class);
    //1.1 Destroy cdev
    cdev_del(&mdev.m_cdev);
    //1.0 Destroy allocate chrdev region
    unregister_chrdev_region(mdev.dev_num,1);
    printk(KERN_INFO "goodbye\n");
    

}

module_init(m_driver_init);
module_exit(m_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
MODULE_INFO(board,"Beaglebone black Rev.C");
MODULE_VERSION("1.0");