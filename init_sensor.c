#include <linux/dev_printk.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

#include "common.h"
#include "init_sensor.h"

int am2303_init(struct platform_device *pdev) {
  struct am2303_data *data;

  // Memory allocated with this function is automatically freed on driver
  //   detach.
  data = devm_kzalloc(&pdev->dev, sizeof(struct am2303_data), GFP_KERNEL);
  if (!data)
    return -ENOMEM;

  // Resources allocated by this function are automatically freed on driver
  //   detach.
  data->gpio = devm_gpiod_get(
      &pdev->dev,
      // This value needs to match xxx-gpios name in device in dts file.
      "data", 0);
  if (IS_ERR(data->gpio)) {
    return dev_err_probe(&pdev->dev, PTR_ERR(data->gpio),
                         "Failed to get GPIO\n");
  }

  data->pdev = pdev;

  // This functions sets device data so it can be acessed from anwyehre in this
  //   device ctx.
  platform_set_drvdata(pdev, data);

  return 0;
}
