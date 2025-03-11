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

    // We need to wait untill bit transmission starts
    udelay(50 - (i == 0 ? 10 : 0));

    // High voltage indicate bit transmission start
    if (gpiod_get_value(data->gpio) != 1) {
      LKM_PRINT_ERR(data->pdev, "Sensor is not pulling up voltage: i=%i\n", i);
      return 1;
    }

    // Now we need to receive one bit, if voltage was high for more than 70us it
    // is 1 if shorter it is 0
    int waits = 0;
    while (gpiod_get_value(data->gpio) && ++waits < 10) {
      udelay(10);
    }

    if (waits >= 7) {
      result[i] = 1;
    } else {
      result[i] = 0;
    }
  }

  dev_info(&data->pdev->dev, "Received:\n");
  for (int i = 0; i < sizeof(result) / sizeof(int); i++) {
    dev_info(&data->pdev->dev, "%i \n", result[i]);
  }

  return 0;
};
