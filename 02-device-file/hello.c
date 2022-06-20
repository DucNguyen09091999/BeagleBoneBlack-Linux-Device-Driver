#include <linux/module.h>  /* Thu vien nay dinh nghia cac macro nhu module_init/module_exit */
#include <linux/fs.h>      /* Thu vien nay dinh nghia cac ham allocate major/minor number */
#include <linux/device.h>  /* Thu vien nay dinh nghia cac ham class_create/device_create */




#define DRIVER_AUTHOR "DucNguyen nguyenchiduc090999@gmail.com"
#define DRIVER_DESC   "Hello world kernel module"

#define Char_Name "m-cdev"
#define Class_Name "m-class"
#define Device_Name "m-device"

struct m_foo_dev
{
    dev_t dev_num;
    struct class *m_class; //Create class
}mdev;


static int
__init hello_world_init(void){
    //1.1 Allocate device number
    if(alloc_chrdev_region(&mdev.dev_num,0,1,Char_Name) < 0){
        pr_err("allocate chrdev region failed\n");
        return -1;
    }

    pr_info("Major = %d, Minor = %d\n",MAJOR(mdev.dev_num),MINOR(mdev.dev_num));

    //1.2 Create struct class
    if((mdev.m_class = class_create(THIS_MODULE,Class_Name)) == NULL){
        pr_err("Can't create the struct class for my device\n");
        goto rm_device_numb;
    }

    //1.3 Create device 
    if((device_create(mdev.m_class,NULL,mdev.dev_num,NULL,Device_Name)) == NULL){
        pr_err("can't create my device\n");
        goto rm_class;
    }
    
    pr_info("Hello world kernel module\n");
    return 0;

    rm_class:
        class_destroy(mdev.m_class);
    rm_device_numb:
        unregister_chrdev_region(mdev.dev_num,1);
        return -1;

}


static void
__exit hello_world_exit(void){
    //1.3 Destroy device
    device_destroy(mdev.m_class,mdev.dev_num);

    //1.2 Destroy class
    class_destroy(mdev.m_class);
    //1.1 Destroy allocate driver number
    unregister_chrdev_region(mdev.dev_num,1);

    pr_info("Goodbye\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION("1.0");


