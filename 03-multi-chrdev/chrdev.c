#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h> //kmalloc
#include <linux/kdev_t.h>
#define DRIVER_AUTHOR "DucNguyen nguyenchiduc090999@gmail.com"
#define DRIVER_DESCRIPTION "character driver kernel module"

#define CDEV_NAME "m-cdev"
#define CLASS_NAME "m-class"


#define NO_OF_DEV 4
#define DEV_MEM_SIZE       1024
#define MAX_MEM_SIZE_MDEV1 1024
#define MAX_MEM_SIZE_MDEV2 1024
#define MAX_MEM_SIZE_MDEV3 1024
#define MAX_MEM_SIZE_MDEV4 1024

//Read-write permission
#define RDONLY  0x01
#define WRONLY  0x10
#define RDWR    0x11

//Define serial number
#define SERIAL_NUMB_MDEV1 "MDEV1"
#define SERIAL_NUMB_MDEV2 "MDEV2"
#define SERIAL_NUMB_MDEV3 "MDEV3"
#define SERIAL_NUMB_MDEV4 "MDEV4"
//Pseudo device's memory
#define NPAGES  1
char dev_buf_mdev1[MAX_MEM_SIZE_MDEV1];
char dev_buf_mdev2[MAX_MEM_SIZE_MDEV2];
char dev_buf_mdev3[MAX_MEM_SIZE_MDEV3];
char dev_buf_mdev4[MAX_MEM_SIZE_MDEV4];

//Device private data structure
struct mdev_private_data
{
    char *buffer;
    unsigned int size;
    const char *serial_number;
    int perm; //permission
    struct  cdev m_cdev;

    
};

//Driver private data structure
struct mdrv_private_data
{
    int size;
    int total_devices;
    dev_t dev_num;
    struct mdev_private_data mdev_data[NO_OF_DEV];
    struct class *m_class;
    struct device *m_device;
    char *kmalloc_ptr;
    
};

struct mdrv_private_data mdrv_data =
{
    .total_devices = NO_OF_DEV,
    .mdev_data = {
        [0] = {
            .buffer = dev_buf_mdev1,
            .size = MAX_MEM_SIZE_MDEV1,
            .serial_number = SERIAL_NUMB_MDEV1,
            .perm = RDONLY,
        },
        [1] = {
            .buffer = dev_buf_mdev2,
            .size = MAX_MEM_SIZE_MDEV2,
            .serial_number = SERIAL_NUMB_MDEV2,
            .perm = WRONLY,
        },
        [2] = {
            .buffer = dev_buf_mdev3,
            .size = MAX_MEM_SIZE_MDEV3,
            .serial_number = SERIAL_NUMB_MDEV3,
            .perm = RDWR,
        },
        [3] = {
            .buffer = dev_buf_mdev4,
            .size = MAX_MEM_SIZE_MDEV4,
            .serial_number = SERIAL_NUMB_MDEV4,
            .perm = RDWR,
        },

    }
};


static int __init m_driver_init(void);
static void __exit m_driver_exit(void);
static loff_t m_lseek (struct file *filp, loff_t offset, int whence);
static ssize_t m_read (struct file *filp, char __user *buff, size_t size, loff_t *offset);
static ssize_t m_write (struct file *filp, const char __user *buff, size_t size, loff_t *offset);
static int m_open (struct inode *inode, struct file *filp);
static int m_release (struct inode *inode, struct file *filp);
static int check_permission(int dev_permission,int access_mode);


static struct file_operations m_fops =
{
    .owner      = THIS_MODULE,
    .read       = m_read,
    .write      = m_write,
    .open       = m_open,
    .release    = m_release,
    .llseek     = m_lseek,
};


static int check_permission(int dev_permission,int access_mode){
    //Read and write permission
    if(dev_permission == RDWR) return 0;
    //make sure that only read access
    if((dev_permission == RDONLY) && (access_mode & FMODE_READ) && !(access_mode & FMODE_WRITE)) return 0;
    if((dev_permission == WRONLY) && (access_mode & FMODE_WRITE) && !(access_mode & FMODE_READ)) return 0;

    return -EPERM;
};

static loff_t m_lseek (struct file *filp, loff_t offset, int whence)
{
    struct mdev_private_data *mdev_data = (struct mdev_private_data*)filp->private_data;
    unsigned int max_size = mdev_data->size;
    loff_t temp;
    pr_info("[INFO] System call lseek() called...\n");
    pr_info("lseek requested\n");
    pr_info("current value of the file position = %lld\n",filp->f_pos);

    switch (whence)
    {
    case SEEK_SET:
        if((offset > max_size ) || (offset < 0)) return -EINVAL;
        filp->f_pos = offset;
        break;
    case SEEK_CUR:
        temp = filp->f_pos + offset;
        if((temp > max_size ) || (temp <0 )) return -EINVAL;
        filp->f_pos = temp;
        break;
    case SEEK_END:
        temp = max_size  + offset;
        if((temp > max_size ) || (temp < 0)) return -EINVAL;
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
    int to_read;
    struct mdev_private_data *mdev_data = (struct mdev_private_data*)filp->private_data;
    int max_size = mdev_data->size;
    pr_info("[INFO] System call read() called...\n");
    //pr_info("Read request for %zu bytes\n",size);
    pr_info("Current file position = %lld\n",*offset);
    //Check Size doesn't exceed our mapped area size
    
    to_read = (*offset + size > max_size) ? (max_size - *offset) : size;
    //to_read = (size > max_size - *offset) ? (max_size - *offset) : size;
    //Copy to user
    if(copy_to_user(buff,mdrv_data.kmalloc_ptr + *offset,to_read)) return -EFAULT;
    //Update file position
    //pr_info("Number of bytes successfully read = %zu\n",size);
    pr_info("Data to usr: %s", mdrv_data.kmalloc_ptr);
    *offset += to_read;
    max_size = *offset;
 
    pr_info("Updated file position = %lld\n",*offset);
    //Return number of bytes which have been successfully read
    return to_read;
}
static ssize_t m_write (struct file *filp, const char __user *buff, size_t size, loff_t *offset)
{
    size_t to_write;
    struct mdev_private_data *mdev_data = (struct mdev_private_data*)filp->private_data;
    int max_size = mdev_data->size;
    pr_info("[INFO] System call write() called...\n");
    //pr_info("Read request for %zu bytes\n",size);
    pr_info("Current file position = %lld\n",*offset);

    // Check size doesn't exceed our mapped area size
	to_write = (size + *offset > max_size) ? (max_size - *offset) : size;

	/* Copy from user buffer to mapped area */
	memset(mdrv_data.kmalloc_ptr, 0, max_size);
	if (copy_from_user(mdrv_data.kmalloc_ptr + *offset, buff, to_write) != 0)
		return -EFAULT;

    pr_info("Data from usr: %s", mdrv_data.kmalloc_ptr);

    *offset += to_write;
    max_size = *offset;
    pr_info("Current file position = %lld\n",*offset);
	return to_write;
}
static int m_open (struct inode *inode, struct file *filp)
{
    int ret,minor_no;
    struct  mdev_private_data *mdev_data;

    
    pr_info("[INFO] System call open() called...\n");
    //Find out on which device file open was attemped by the user space
    minor_no = MINOR(inode->i_rdev);
    pr_info("Minor access =%d\n",minor_no);

    //Get device's private data
    mdev_data = container_of(inode->i_cdev,struct mdev_private_data,m_cdev);

    //To supply device private data to other methods of the driver
    filp->private_data =mdev_data;

    //Check permission
    ret = check_permission(mdev_data->perm,filp->f_mode);
    pr_info("check permission %d\n",ret);
    if(!ret)
        pr_info("Open was successful\n");
    else
         pr_info("Open was unsucessful\n");
    return ret;
}
static int m_release (struct inode *inode, struct file *filp)
{
    pr_info("[INFO] System call release() called...\n");
    return 0;
}

static int
__init m_driver_init(void){
    int i = 0;
        //1.0 dynamic allocate a device number
    if((alloc_chrdev_region(&mdrv_data.dev_num,0,NO_OF_DEV,CDEV_NAME)) < 0){
        pr_err("Can't allocate chrdev region\n");
        return -1;
    }
        //1.3 Create device class under /sys/class/
    if((mdrv_data.m_class = class_create(THIS_MODULE,CLASS_NAME)) == NULL){
        pr_err("Can't create device class for my device\n");
        goto rm_dev_numb;
    }

    for(i =0; i < NO_OF_DEV;i++){
        pr_info("[INFO] Major = %d, Minor = %d\n",MAJOR(mdrv_data.dev_num +i),MINOR(mdrv_data.dev_num+i));
         //1.1 Initialize cdev structure with fops
        cdev_init(&mdrv_data.mdev_data[i].m_cdev,&m_fops);
    
        //1.2 Register cdev structure with VFS
        mdrv_data.mdev_data[i].m_cdev.owner = THIS_MODULE;
        if(cdev_add(&mdrv_data.mdev_data[i].m_cdev,mdrv_data.dev_num + i,1) < 0){
            pr_err("Can't register cdev structure to the system\n");
            goto rm_cdev;
        }

        //1.4 Creating device
        if((device_create(mdrv_data.m_class,NULL,mdrv_data.dev_num + i,NULL,"m_device_%d",i+1)) == NULL){
        pr_err("Can't create my device\n");
        goto rm_class;
        }
    }
    

       

    //1.5 Creating physical memory
    if((mdrv_data.kmalloc_ptr =kmalloc(DEV_MEM_SIZE,GFP_KERNEL)) == 0){
        pr_err("Can't allocate memory in kernel\n");
        goto rm_cdev;
    }

    printk(KERN_INFO "Hello world kernel module\n");
    return 0;

    rm_cdev:
    rm_class:
        for(;i>=0;i--){
            device_destroy(mdrv_data.m_class,mdrv_data.dev_num + i);
            cdev_del(&mdrv_data.mdev_data[i].m_cdev);
        }
        class_destroy(mdrv_data.m_class);
    rm_dev_numb:
        unregister_chrdev_region(mdrv_data.dev_num,NO_OF_DEV);
        return -1;

}

static void
__exit m_driver_exit(void){
    int i;
    kfree(mdrv_data.kmalloc_ptr);  

    for(i=0;i < NO_OF_DEV;i++){
         //1.3 Destroy device
    device_destroy(mdrv_data.m_class,mdrv_data.dev_num + i);
    //1.1 Destroy cdev
    cdev_del(&mdrv_data.mdev_data[i].m_cdev);
    }
   
    //1.2 Destroy class
    class_destroy(mdrv_data.m_class);
    //1.0 Destroy allocate chrdev region
    unregister_chrdev_region(mdrv_data.dev_num,NO_OF_DEV);
    printk(KERN_INFO "goodbye\n");
    

}

module_init(m_driver_init);
module_exit(m_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
MODULE_INFO(board,"Beaglebone black Rev.C");
MODULE_VERSION("1.0");