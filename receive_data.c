#include <linux/delay.h>
#include <linux/dev_printk.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

#include "asm-generic/delay.h"
#include "common.h"

int simple_sensor_cache_receive_data(struct simple_sensor_cache_data *data) {
  // At this point pin is already set to receive

  // We need to obtain 40 bits, bit by bit.
  int result[40];

  for (int i = 0; i < sizeof(result) / sizeof(int); i++) {
    // First sensor pulls down voltage to indicate bit transmission
    if (gpiod_get_value(data->gpio) != 0) {
      LKM_PRINT_ERR(data->pdev, "Sensor is not pulling down voltage: i=%i\n",
                    i);
      return 1;
    }

    // We need to wait for the voltage to be pulled up
    int waits = 0;
    while (!gpiod_get_value(data->gpio) && ++waits < 100) {
      udelay(1);
    }

    if (waits >= 100) {
      LKM_PRINT_ERR(data->pdev,
                    "Sensor is not pulling up voltage: waits=%i, i=%i\n", waits,
                    i);
      return 1;
    }

    // Now we need to receive one bit, if voltage was high for more than 70us it
    // is 1 if shorter it is 0
    waits = 0;
    while (gpiod_get_value(data->gpio) && ++waits < 100) {
      udelay(10);
    }

    if (2 <= waits && waits <= 4) { // If between 20us and 40us it's 0
      result[i] = 0;
    } else if (6 <= waits && waits <= 8) { // If between 60us and 80us it's 1
      result[i] = 1;
    } else { // If longer than 80us it's unrecognized value
      result[i] = -1;
    }
  }

  dev_info(&data->pdev->dev, "Received:\n");
  for (int i = 0; i < sizeof(result) / sizeof(int); i++) {
    dev_info(&data->pdev->dev, "%i \n", result[i]);
  }

  return 0;
};
