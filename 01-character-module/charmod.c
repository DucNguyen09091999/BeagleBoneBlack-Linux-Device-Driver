#include <linux/module.h>  /* Thu vien nay dinh nghia cac macro nhu module_init/module_exit */
#include <linux/fs.h>      /* Thu vien nay dinh nghia cac ham allocate major/minor number */
// #include <linux/device.h>  /* Thu vien nay dinh nghia cac ham class_create/device_create */
// #include <linux/cdev.h>    /* Thu vien nay dinh nghia cac ham kmalloc */
// #include<linux/slab.h>     /* Thu vien nay dinh nghia cac ham cdev_init/cdev_add */
// #include<linux/uaccess.h>  /* Thu vien nay dinh nghia cac ham copy_to_user/copy_from_user */



#define DRIVER_AUTHOR "DucNguyen nguyenchiduc090999@gmail.com"
#define DRIVER_DESC   "Hello world kernel module"

#define Driver_Name "m-cdev"

struct m_foo_dev
{
    dev_t dev_num;
}mdev;

static int
__init hello_world_init(void){

    //1.1 Dynamic allocate 
    if(alloc_chrdev_region(&mdev.dev_num,0,1,Driver_Name) < 0){
        pr_err("Failed to alloc chrdev region\n");
        return -1;
    }

    pr_info("Major = %d Minor = %d\n",MAJOR(mdev.dev_num),MINOR(mdev.dev_num));
    pr_info("Hello world kernel module\n");
    return 0;

}


static void
__exit hello_world_exit(void){
    unregister_chrdev_region(mdev.dev_num,1);
    pr_info("Goodbye\n");
}


module_init(hello_world_init);
module_exit(hello_world_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION("1.0");
