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

  for (int i = 0; i < 8; i++) {
    data->humidity_number = (data->humidity_number << 1) | result[i];
  }

  for (int i = 8; i < 16; i++) {
    data->humidity_fraction = (data->humidity_fraction << 1) | result[i];
  }

  for (int i = 16; i < 24; i++) {
    data->temp_number = (data->temp_number << 1) | result[i];
  }

  for (int i = 24; i < 32; i++) {
    data->temp_fraction = (data->temp_fraction << 1) | result[i];
  }

  for (int i = 32; i < 40; i++) {
    data->checksum = (data->checksum << 1) | result[i];
  }

  dev_info(&data->pdev->dev, "Humidity: %d.%d\n", data->humidity_number,
           data->humidity_fraction);
  dev_info(&data->pdev->dev, "Temprature: %d.%d\n", data->temp_number,
           data->temp_fraction);
  dev_info(&data->pdev->dev, "Checksum: %d vs %d\n",
           (data->humidity_number + data->humidity_fraction +
            data->temp_number + data->temp_fraction) &
               0xFF,
           data->checksum);

  return 0;
};
