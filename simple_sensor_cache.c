/*
 * simple_sensor_cache.c
 ****************************************************************
 * Brief Description:
 * A simple module which implements driver for AM2303 and some caching
 *  mechanism. AM2303 uses some custom single-bus protocol.
 */
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio.h>          // For gpio_to_desc()
#include <linux/gpio/consumer.h> // For descriptor-based GPIO
#include <linux/gpio/consumer.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>

struct simple_sensor_cache {
  struct gpio_desc *gpio;
};

static int simple_sensor_cache_probe(struct platform_device *pdev) {
  struct simple_sensor_cache *ow;

  ow = devm_kzalloc(&pdev->dev, sizeof(*ow), GFP_KERNEL);
  if (!ow)
    return -ENOMEM;

  ow->gpio = devm_gpiod_get(&pdev->dev, "data", GPIOD_OUT_HIGH);
  if (IS_ERR(ow->gpio))
    return dev_err_probe(&pdev->dev, PTR_ERR(ow->gpio), "Failed to get GPIO\n");

  platform_set_drvdata(pdev, ow);
  dev_info(&pdev->dev, "Custom one-wire GPIO driver probed\n");

  return 0;
}

static int simple_sensor_cache_remove(struct platform_device *pdev) {
  dev_info(&pdev->dev, "Custom one-wire GPIO driver removed\n");
  return 0;
}

static const struct of_device_id simple_sensor_cache_of_match[] = {
    {
        .compatible = "raspberrypi,simple_sensor_cache_device",
    },
    {},
};
MODULE_DEVICE_TABLE(of, simple_sensor_cache_of_match);

static struct platform_driver simple_sensor_cache_driver = {
    .probe = simple_sensor_cache_probe,
    .remove = simple_sensor_cache_remove,
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
