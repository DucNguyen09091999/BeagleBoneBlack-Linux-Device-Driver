/**
 ******************************************************************************
 * @file           : hello.c
 * @author         : Duc Nguyen
 * @brief          : Hello world kernel module
 * @ver            : 1.0
 ******************************************************************************
 */

#include <linux/module.h>


#define DRIVER_AUTHOR "DucNguyen nguyenchiduc090999@gmail.com"
#define DRIVER_DESC "Hello world kernel module"
#define DRIVER_VERS "1.0"


static int
__init hello_world_init(void){
    printk(KERN_INFO "Hello world kernel module\n");
    return 0;

}

static void
__exit hello_world_exit(void){
    printk(KERN_INFO "Goodbye\n");
}



module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_VERSION(DRIVER_VERS);
MODULE_INFO(board,"Beaglebone Black Rev.C");