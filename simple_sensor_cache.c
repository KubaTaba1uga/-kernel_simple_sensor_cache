/*
 * simple_sensor_cache.c
 ****************************************************************
 * Brief Description:
 * A simple module which implements driver for AM2303 and some caching
 *  mechanism. AM2303 uses some custom single-bus protocol.
 */
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

struct simple_sensor_cache_data {
  struct gpio_desc *gpio;
};

static int simple_sensor_cache_probe(struct platform_device *pdev) {
  static struct simple_sensor_cache_data *data;

  // Memory allocated with this function is automatically freed on driver
  //   detach.
  data = devm_kzalloc(&pdev->dev, sizeof(struct simple_sensor_cache_data),
                      GFP_KERNEL);
  if (!data)
    return -ENOMEM;

  // Resources allocated by this function are automatically freed on driver
  //   detach.
  data->gpio = devm_gpiod_get(
      &pdev->dev,
      // This value needs to match xxx-gpios name in device in dts file.
      "data", GPIOD_IN);
  if (IS_ERR(data->gpio)) {
    return dev_err_probe(&pdev->dev, PTR_ERR(data->gpio),
                         "Failed to get GPIO\n");
  }

  // This functions sets device data so it can be acessed from anwyehre in this
  //   device ctx.
  platform_set_drvdata(pdev, data);

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

MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Custom GPIO descriptor-based one-wire driver");
MODULE_LICENSE("GPL");
