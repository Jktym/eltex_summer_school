#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("ELTEX_SUMMER_SCHOOL");
MODULE_AUTHOR("Yerkenov_Daniyar");
MODULE_DESCRIPTION("A simple hello world module");

static int __init hello_init(void) {
    printk(KERN_INFO "Hello kernel World!\n");
    return 0;
}

static void __exit hello_cleanup(void) {
    printk(KERN_INFO "Cleaning up Hello module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);