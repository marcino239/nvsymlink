
/*  creates a simlink from /sys/device/soc0/... to  /sys/device/platform
 *
 *  Copyright (C) 2014 by marcino239    github.com/marcino239
 *
 */

#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>

#define NVLINK_DEV_NAME "nvsymlink" 
#define NVLINK_LINK_NAME "host1x"

#define MAX_HANDLES 16

// structures
struct handle {
    struct device *dev;
    struct kobject *kobj;
    const unsigned char *linkname;
};


// main platform structure
static struct platform_device *plat_dev;
static struct handle handles[ MAX_HANDLES ];
static int handle_count;


int add_handle( struct device *dev, struct kobject *kobj, const unsigned char *linkname )
{
    if( handle_count >= MAX_HANDLES - 1 ) {
        printk( KERN_ALERT NVLINK_DEV_NAME ": too many handles stored\n");
        return -ENOMEM;
    }

    handle_count += 1;
    handles[ handle_count ].dev = dev;
    handles[ handle_count ].kobj = kobj;
    handles[ handle_count ].linkname = linkname;

    return 0;
}

void free_handles( void )
{
    while( handle_count >= 0 ) {
        if( handles[ handle_count ].dev )
            put_device( handles[ handle_count ].dev );

        if( handles[ handle_count ].kobj )
            sysfs_remove_link( handles[ handle_count ].kobj, handles[ handle_count ].linkname );

        handle_count -= 1;
    }
}


// creates link to a device
int dev_create_link( struct kobject *kmaindir, struct kobject *target, const unsigned char *linkname )
{
    int ret;

    // create link
    ret = sysfs_create_link( kmaindir, target, linkname );
    if( ret ) {
        printk( NVLINK_DEV_NAME ": unable to create link: %s\n", linkname );
        return ret;
    }

    ret = add_handle( NULL, kmaindir, linkname );
    if( ret )
        free_handles();

    return ret;
}


// find and creates link to a device
int dev_find_and_create_link( const unsigned char *devname, struct kobject *kmaindir, const unsigned char *linkname )
{
    int ret;
    struct device *dev;

    dev = bus_find_device_by_name( &platform_bus_type, NULL, devname );
    if( dev == NULL ) {
        printk( NVLINK_DEV_NAME ": device %s FOUND\n", devname );
        return -ENOENT;
    }
    printk( NVLINK_DEV_NAME ": device %s FOUND\n", devname );

    ret = sysfs_create_link( kmaindir, &dev->kobj, linkname );
    if( ret ) {
        printk( NVLINK_DEV_NAME ": unable to create link: %s\n", linkname );
        put_device( dev );
        return ret;
    }

    ret = add_handle( dev, kmaindir, linkname );
    if( ret )
        free_handles();

    return ret;
}

// init
static int __init nvlink_module_init( void )
{
    int ret;

    // init structures
    plat_dev = NULL;
    memset( handles, 0, sizeof( handles ) );
    handle_count = -1;

    printk( NVLINK_DEV_NAME " init\n" );

    plat_dev = platform_device_register_simple( NVLINK_DEV_NAME, PLATFORM_DEVID_NONE, NULL, 0 );
    if( IS_ERR( plat_dev )) {
        ret = PTR_ERR( plat_dev );
        return ret;
    }

    // create symlink between platform and NVLINK_NAME
    ret = dev_create_link( &plat_dev->dev.parent->kobj, &plat_dev->dev.kobj, NVLINK_LINK_NAME );
    if( ret )
        goto cleanup_1;

    // find device
    ret = dev_find_and_create_link( "57000000.gk20a", &plat_dev->dev.kobj, "gk20a.0" );
    if( ret )
        goto cleanup_1;

    return 0;

cleanup_1:
    printk(KERN_ALERT NVLINK_DEV_NAME ": cleanning up\n");

    free_handles();
    platform_device_unregister( plat_dev );
    return ret;
}


static void __exit nvlink_module_exit(void)
{
    printk(KERN_ALERT NVLINK_DEV_NAME " exit\n");

    free_handles();
    platform_device_unregister( plat_dev );
}

module_init(nvlink_module_init);
module_exit(nvlink_module_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("marcino239 github.com/marcino239");
