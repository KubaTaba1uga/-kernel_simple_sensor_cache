#include <linux/device.h>

#include "common.h"
#include "receive_data.h"
#include "set_up_communication.h"

static ssize_t show_temp(struct device *dev, struct device_attribute *attr,
                         char *buf);
static ssize_t show_humid(struct device *dev, struct device_attribute *attr,
                          char *buf);

static DEVICE_ATTR(temp, 0440, show_temp, NULL);
static DEVICE_ATTR(humid, 0440, show_humid, NULL);

int simple_sensor_cache_init_sysfs(struct simple_sensor_cache_data *data) {
  int err;

  err = device_create_file(&data->pdev->dev, &dev_attr_temp);
  if (err) {
    return err;
  }

  err = device_create_file(&data->pdev->dev, &dev_attr_humid);
  if (err) {
    return err;
  }

  return 0;
};

void simple_sensor_cache_destroy_sysfs(struct simple_sensor_cache_data *data) {
  device_remove_file(&data->pdev->dev, &dev_attr_temp);
  device_remove_file(&data->pdev->dev, &dev_attr_humid);
};

static ssize_t show_temp(struct device *dev, struct device_attribute *attr,
                         char *buf) {
  struct simple_sensor_cache_data *data;
  int err;

  data = dev_get_drvdata(dev);

  err = simple_sensor_cache_set_up_communication(data);
  if (err) {
    LKM_PRINT_ERR(data->pdev, "Unable to set up communication with sensor");
    return -1;
  }

  err = simple_sensor_cache_receive_data(data);
  if (err) {
    LKM_PRINT_ERR(data->pdev, "Unable to receive data from sensor");
    return -1;
  }

  return sprintf(buf, "%d\n", data->temprature);
}

static ssize_t show_humid(struct device *dev, struct device_attribute *attr,
                          char *buf) {
  struct simple_sensor_cache_data *data;
  int err;

  data = dev_get_drvdata(dev);

  err = simple_sensor_cache_set_up_communication(data);
  if (err) {
    LKM_PRINT_ERR(data->pdev, "Unable to set up communication with sensor");
    return -1;
  }

  err = simple_sensor_cache_receive_data(data);
  if (err) {
    LKM_PRINT_ERR(data->pdev, "Unable to receive data from sensor");
    return -1;
  }

  return sprintf(buf, "%d\n", data->humidity);
}
