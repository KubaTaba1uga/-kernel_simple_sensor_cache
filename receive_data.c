#include <linux/delay.h>
#include <linux/dev_printk.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

#include "common.h"

int simple_sensor_cache_receive_data(struct simple_sensor_cache_data *data) {
  // At this point pin is already set to receive

  // First sensor pulls down voltage to indicate bit transmission
  int i = 0;
  while (gpiod_get_value(data->gpio) && ++i < 10) {
    usleep_range(20, 40);
  }

  if (i == 10) {
    dev_err(&data->pdev->dev, "Sensor is not responding with low voltage: %i\n",
            i);
    return 1;
  }

  // We need to wait untill bit transmission starts
  udelay(50);

  // Now we need to receive one bit, if voltage was high for more than 70us it
  // is 1 if shorter it is 0
  i = 0;
  while (gpiod_get_value(data->gpio) && ++i < 10) {
    udelay(10);
  }

  if (i >= 7) {
    dev_info(&data->pdev->dev, "Received one\n");
  } else {
    dev_info(&data->pdev->dev, "Received zero\n");
  }

  return 0;
};
