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

#include "init_sensor.h"
#include "receive_data.h"
#include "set_up_communication.h"

/***************************************************************
 *                        PUBLIC API
 **************************************************************/
static int simple_sensor_cache_probe(struct platform_device *pdev) {
  static struct simple_sensor_cache_data *data;
  int err;

  dev_info(&pdev->dev, "Probing...\n");

  err = simple_sensor_cache_init(pdev);
  if (err) {
    dev_err(&pdev->dev, "Unable to init the driver\n");
    return err;
  }

  data = platform_get_drvdata(pdev);

  err = simple_sensor_cache_set_up_communication(data);
  if (err) {
    dev_err(&pdev->dev, "Unable to set up communication\n");
    return err;
  }

  err = simple_sensor_cache_receive_data(data);
  if (err) {
    dev_err(&pdev->dev, "Unable to receive data\n");
  }

  dev_info(&pdev->dev, "Custom one-wire GPIO driver probed\n");

  return 0;
}

static void simple_sensor_cache_remove(struct platform_device *pdev) {
  dev_info(&pdev->dev, "Custom one-wire GPIO driver removed\n");
}

static const struct of_device_id simple_sensor_cache_of_match[] = {
    {
        // This is a unique value which should match `compatibile` field in
        // overlay.
        .compatible = "raspberrypi,simple_sensor_cache_device",
    },
    {},
};
MODULE_DEVICE_TABLE(of, simple_sensor_cache_of_match);

static struct platform_driver simple_sensor_cache_driver = {
    .probe = simple_sensor_cache_probe,
    .remove_new = simple_sensor_cache_remove,
    .driver =
        {
            .name = "simple_sensor_cache_gpio",
            .of_match_table = simple_sensor_cache_of_match,
        },
};

module_platform_driver(simple_sensor_cache_driver);

MODULE_AUTHOR("Jakub Buczynski");
MODULE_DESCRIPTION("Custom GPIO descriptor-based one-wire driver for AM2303");
MODULE_LICENSE("GPL");
