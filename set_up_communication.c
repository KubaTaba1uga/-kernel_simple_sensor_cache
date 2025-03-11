#include "linux/dev_printk.h"
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

#include "common.h"

int simple_sensor_cache_set_up_communication(
    struct simple_sensor_cache_data *data) {
  // First we need to drive output high to initialize communication
  gpiod_direction_output(data->gpio, 1);
  udelay(500); // Busy-wait some time to allow sensor to detect init

  // Set the GPIO low to continue initialization
  gpiod_set_value(data->gpio, 0);
  mdelay(20); // Busy-wait for >18ms

  // Set the GPIO back to high and wait for response
  gpiod_set_value(data->gpio, 1);
  udelay(30); // Wait between 20 and 40 us
  gpiod_direction_input(data->gpio);

  udelay(20);
  if (gpiod_get_value(data->gpio) != 0) {
    dev_err(&data->pdev->dev, "Sensor is not responding with low voltage\n");
    return 1;
  }
  udelay(30);

  udelay(50);
  if (gpiod_get_value(data->gpio) != 1) {
    dev_err(&data->pdev->dev, "Sensor is not responding with high voltage\n");
    return 1;
  }

  return 0;
};
