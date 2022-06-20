#include <linux/module.h> //module_init + module_exit
#include <linux/fs.h> //allocate major + minor
#include <linux/device.h> //class_create + device_create
#include <linux/cdev.h>    /* Thu vien nay dinh nghia cac ham kmalloc */
#include<linux/slab.h>     /* Thu vien nay dinh nghia cac ham cdev_init/cdev_add */
#include<linux/uaccess.h>  /* Thu vien nay dinh nghia cac ham copy_to_user/copy_from_user */

#define DRIVER_AUTHOR "DucNguyen nguyenchiduc090999@gmail.com"
#define DRIVER_DESC "File operation kernel module"

#define CDEV_NAME "m-cdev"
#define CLASS_NAME "m-class"
#define DEVICE_NAME "m-dev"

#define NPAGES  1

struct m_foo_dev{
    dev_t dev_num;
    struct class *m_class;
    struct cdev m_cdev;
    int32_t size;
    char *kmalloc_ptr;
}mdev;


// Declare Function prototype

static int      __init file_oper_init(void);
static void     __exit file_oper_exit(void);
static int      m_open(struct inode *inode, struct file *file);
static int      m_release(struct inode *inode, struct file *file);
static ssize_t  m_read(struct file *filp, char __user *user_buf, size_t size,loff_t * offset);
static ssize_t  m_write(struct file *filp, const char *user_buf, size_t size, loff_t * offset);

static struct file_operations fops =
{
    .owner      = THIS_MODULE,
    .read       = m_read,
    .write      = m_write,
    .open       = m_open,
    .release    = m_release,
};

/* This function will be called when we open the Device file */
static int m_open(struct inode *inode, struct file *file)
{
    pr_info("System call open() called...!!!\n");
    return 0;
}

/* This function will be called when we close the Device file */
static int m_release(struct inode *inode, struct file *file)
{
    pr_info("System call close() called...!!!\n");
    return 0;
}

/* This function will be called when we read the Device file */
static ssize_t m_read(struct file *filp, char __user *user_buf, size_t size, loff_t *offset)
{

    size_t to_read;

    pr_info("System call read() called...!!!\n");

    /* Check size doesn't exceed our mapped area size */
    to_read = (size > mdev.size - *offset) ? (mdev.size - *offset) : size;

	/* Copy from mapped area to user buffer */
	if (copy_to_user(user_buf, mdev.kmalloc_ptr + *offset, to_read))
		return -EFAULT;

    pr_info("Data for user: %s",mdev.kmalloc_ptr);
    *offset += to_read;

	return to_read;
   
}

/* This function will be called when we write the Device file */
static ssize_t m_write(struct file *filp, const char __user *user_buf, size_t size, loff_t *offset)
{

    size_t to_write;

    pr_info("System call write() called...!!!\n");

    /* Check size doesn't exceed our mapped area size */
	to_write = (size + *offset > NPAGES * PAGE_SIZE) ? (NPAGES * PAGE_SIZE - *offset) : size;

	/* Copy from user buffer to mapped area */
	memset(mdev.kmalloc_ptr, 0, NPAGES * PAGE_SIZE);
	if (copy_from_user(mdev.kmalloc_ptr + *offset, user_buf, to_write) != 0)
		return -EFAULT;

    pr_info("Data from usr: %s", mdev.kmalloc_ptr);

    *offset += to_write;
    mdev.size = *offset;

	return to_write;

}


static int
__init file_oper_init(void){

    //1.1 Allocate device numb
    if(alloc_chrdev_region(&mdev.dev_num,0,1,CDEV_NAME) < 0){
        pr_err("Allocate device region failed\n");
        return -1;
    }

    pr_info("Major = %d, Minor = %d\n",MAJOR(mdev.dev_num),MINOR(mdev.dev_num));

    //2.1 Creating cdev structure
    cdev_init(&mdev.m_cdev,&fops);


    //2.2 Adding characteristic device to the system
    if((cdev_add(&mdev.m_cdev,mdev.dev_num,1)) < 0){
        pr_err("Can't add the device to the system\n");
        goto rm_dev_numb;
    }

    //1.2 Create class
    if((mdev.m_class= class_create(THIS_MODULE,CLASS_NAME)) == NULL){
        pr_err("Class create failed\n");
        goto rm_dev_numb;
    }

    //1.3 Create device
    if(device_create(mdev.m_class,NULL,mdev.dev_num,NULL,DEVICE_NAME) == NULL){
        pr_err("Create device failed\n");
        goto rm_class;
    }

    //3.1 Create Physical memory
    if((mdev.kmalloc_ptr = kmalloc(1024 , GFP_KERNEL)) == 0){
        pr_err("Cannot allocate memory in kernel\n");
        goto rm_device;
    }

    pr_info("File operation kernel module\n");
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
__exit file_oper_exit(void){

    //3.1 Destroy kmalloc
    kfree(mdev.kmalloc_ptr);

    //1.3 Destroy device
    device_destroy(mdev.m_class,mdev.dev_num);

    //1.2 Destroy class
    class_destroy(mdev.m_class);

    //2.1 Destroy cdev
    cdev_del(&mdev.m_cdev);

    //1.1 Destroy device numb
    unregister_chrdev_region(mdev.dev_num,1);
    pr_info("Goodbye\n");
}

module_init(file_oper_init);
module_exit(file_oper_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION("1.0");
MODULE_INFO(board, "Beaglebone Black Rev.C");