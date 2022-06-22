#include <linux/module.h>

#define DRIVER_AUTHOR "DucNguyen nguyenchiduc090999@gmail.com"
#define DRIVER_DESCRIPTION "hello world kernel module"


static int
__init hello_world_init(void){
    printk(KERN_INFO "Hello world kernel module\n");
    return 0;
}

static void
__exit hello_world_exit(void){

    printk(KERN_INFO "goodbye\n");

}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESCRIPTION);
MODULE_INFO(board,"Beaglebone black Rev.C");
MODULE_VERSION("1.0");