#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h> //kmalloc
#define DRIVER_AUTHOR "DucNguyen nguyenchiduc090999@gmail.com"
#define DRIVER_DESCRIPTION "character driver kernel module"

#define CDEV_NAME "m-cdev"
#define CLASS_NAME "m-class"
#define DEVICE_NAME "m-device"
#define DEV_MEM_SIZE 1024
//Pseudo device's memory
#define NPAGES  1
struct m_foo_dev
{   
    int size;
    dev_t dev_num;
    struct cdev m_cdev;
    struct class *m_class;
    char *kmalloc_ptr;
}mdev;

static int __init m_driver_init(void);
static void __exit m_driver_exit(void);
static loff_t m_lseek (struct file *filp, loff_t offset, int whence);
static ssize_t m_read (struct file *filp, char __user *buff, size_t size, loff_t *offset);
static ssize_t m_write (struct file *filp, const char __user *buff, size_t size, loff_t *offset);
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

static loff_t m_lseek (struct file *filp, loff_t offset, int whence)
{
    loff_t temp;
    pr_info("[INFO] System call lseek() called...\n");
    pr_info("lseek requested\n");
    pr_info("current value of the file position = %lld\n",filp->f_pos);

    switch (whence)
    {
    case SEEK_SET:
        if((offset > DEV_MEM_SIZE) || (offset < 0)) return -EINVAL;
        filp->f_pos = offset;
        break;
    case SEEK_CUR:
        temp = filp->f_pos + offset;
        if((temp > DEV_MEM_SIZE) || (temp <0 )) return -EINVAL;
        filp->f_pos = temp;
        break;
    case SEEK_END:
        temp = DEV_MEM_SIZE + offset;
        if((temp > DEV_MEM_SIZE) || (temp < 0)) return -EINVAL;
        filp->f_pos = temp;
        break;        
    default:
        return -EINVAL;
    }

    pr_info("New value of the position = %lld\n",filp->f_pos);

    return filp->f_pos;
}
static ssize_t m_read (struct file *filp, char __user *buff, size_t size, loff_t *offset)
{
    size_t to_read;
    pr_info("[INFO] System call read() called...\n");
    //pr_info("Read request for %zu bytes\n",size);
    pr_info("Current file position = %lld\n",*offset);
    //Check Size doesn't exceed our mapped area size
    
    to_read = (size > mdev.size - *offset) ? (mdev.size - *offset) : size;
    //Copy to user
    if(copy_to_user(buff,mdev.kmalloc_ptr + *offset,to_read)) return -EFAULT;
    //Update file position
    *offset += to_read;
    //pr_info("Number of bytes successfully read = %zu\n",size);
    pr_info("Data to usr: %s", mdev.kmalloc_ptr);
    pr_info("Updated file position = %lld\n",*offset);
    //Return number of bytes which have been successfully read
    return to_read;
}
static ssize_t m_write (struct file *filp, const char __user *buff, size_t size, loff_t *offset)
{
    size_t to_write;
    pr_info("[INFO] System call write() called...\n");
    

    // Check size doesn't exceed our mapped area size
	to_write = (size + *offset > NPAGES * PAGE_SIZE) ? (NPAGES * PAGE_SIZE - *offset) : size;

	/* Copy from user buffer to mapped area */
	memset(mdev.kmalloc_ptr, 0, NPAGES * PAGE_SIZE);
	if (copy_from_user(mdev.kmalloc_ptr + *offset, buff, to_write) != 0)
		return -EFAULT;

    pr_info("Data from usr: %s", mdev.kmalloc_ptr);

    *offset += to_write;
    mdev.size = *offset;

	return to_write;
}
static int m_open (struct inode *inode, struct file *filp)
{
    pr_info("[INFO] System call open() called...\n");
    return 0;
}
static int m_release (struct inode *inode, struct file *filp)
{
    pr_info("[INFO] System call release() called...\n");
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

    //1.5 Creating physical memory
    if((mdev.kmalloc_ptr =kmalloc(DEV_MEM_SIZE,GFP_KERNEL)) == 0){
        pr_err("Can't allocate memory in kernel\n");
        goto rm_device;
    }

    printk(KERN_INFO "Hello world kernel module\n");
    return 0;
    rm_device:
        device_destroy(mdev.m_class,mdev.dev_num);
    rm_class:
        class_destroy(mdev.m_class);
    rm_dev_numb:
        unregister_chrdev_region(mdev.dev_num,1);
        return -1;

}

static void
__exit m_driver_exit(void){
    
    kfree(mdev.kmalloc_ptr);  
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