/*
 * simple_sensor_cache.c
 ****************************************************************
 * Brief Description:
 * A simple module which implements driver for AM2303 and some caching
 *  mechanism.
 */

#include "linux/printk.h"
#include <asm-generic/bitsperlong.h>
#include <linux/init.h>
#include <linux/module.h>

MODULE_AUTHOR("KubaTaba1uga");
MODULE_DESCRIPTION("a simple LKM driving AM2303");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("0.1");

static int __init simple_sensor_cache_init(void) {
  pr_info("Inserted\n");
  return 0;
}

static void __exit simple_sensor_cache_exit(void) { pr_info("Removed\n"); }

module_init(simple_sensor_cache_init);
module_exit(simple_sensor_cache_exit);
