/*
 * am2303 driver
 ****************************************************************
 * Brief Description:
 * A simple module which implements driver for AM2303 and some caching
 *  mechanism. AM2303 uses some custom single-bus protocol which requires
 *  that read occurs respecting 2s waiting time. Module implements simple
 *  cache which ensures that reads does not occur more often.
 */
#include <linux/of_device.h>
#include <linux/platform_device.h>

#include "common.h"
#include "init_sensor.h"
#include "receive_data.h"
#include "set_up_communication.h"
#include "sysfs.h"

/***************************************************************
 *                        PUBLIC API
 **************************************************************/
static int am2303_probe(struct platform_device *pdev) {
  struct am2303_data *data;
  int err;

  dev_info(&pdev->dev, "Probing...\n");

  err = am2303_init(pdev);
  if (err) {
    LKM_PRINT_ERR(pdev, "Unable to init the driver\n");
    return err;
  }

  data = platform_get_drvdata(pdev);

  err = am2303_init_sysfs(data);
  if (err) {
    LKM_PRINT_ERR(pdev, "Unable to init sysfs\n");
    return err;
  }

  dev_info(&pdev->dev, "AM2303 probed\n");

  return 0;
}

static void am2303_remove(struct platform_device *pdev) {
  struct am2303_data *data;
  data = platform_get_drvdata(pdev);

  am2303_destroy_sysfs(data);

  dev_info(&pdev->dev, "AM2303 removed\n");
}

static const struct of_device_id am2303_of_match[] = {
    {
        // This is a unique value which should match `compatibile` field in
        // overlay.
        .compatible = "raspberrypi,am2303_device",
    },
    {},
};
MODULE_DEVICE_TABLE(of, am2303_of_match);

static struct platform_driver am2303_driver = {
    .probe = am2303_probe,
    .remove_new = am2303_remove,
    .driver =
        {
            .name = "am2303_gpio",
            .of_match_table = am2303_of_match,
        },
};

module_platform_driver(am2303_driver);

MODULE_AUTHOR("Jakub Buczynski");
MODULE_DESCRIPTION("Custom GPIO descriptor-based one-wire driver for AM2303");
MODULE_LICENSE("GPL");
