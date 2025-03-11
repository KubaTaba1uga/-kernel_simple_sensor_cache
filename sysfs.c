#include <linux/kobject.h>

#include "common.h"
#include "receive_data.h"
#include "set_up_communication.h"

static ssize_t show_temp(struct kobject *kobj, struct kobj_attribute *attr,
                         char *buf);
static ssize_t show_humid(struct kobject *kobj, struct kobj_attribute *attr,
                          char *buf);

static struct kobj_attribute temp_attr =
    __ATTR(temp_attr, 0440, show_temp, NULL);
static struct kobj_attribute humid_attr =
    __ATTR(humid_attr, 0440, show_humid, NULL);

static int counter = 0;

int simple_sensor_cache_init_sysfs(struct simple_sensor_cache_data *data) {
  int err;

  data->kobj =
      kobject_create_and_add("am2303",
                             // We need to create kobj as child of our device
                             //  to retrieve it later by kobj_to_dev.
                             &data->pdev->dev.kobj);
  if (!data->kobj) {
    err = 1;
    goto error;
  }

  err = sysfs_create_file(data->kobj, &temp_attr.attr);
  if (err) {
    goto error;
  }

  err = sysfs_create_file(data->kobj, &humid_attr.attr);
  if (err) {
    goto error;
  }

  return 0;
error:
  kobject_put(data->kobj);
  return err;
};

void simple_sensor_cache_destroy_sysfs(struct simple_sensor_cache_data *data) {
  kobject_put(data->kobj);
};

static ssize_t show_temp(struct kobject *kobj, struct kobj_attribute *attr,
                         char *buf) {
  struct simple_sensor_cache_data *data;
  int err;

  struct device *dev = kobj_to_dev(kobj);
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

  return sprintf(buf, "%d\n", counter++);
}

static ssize_t show_humid(struct kobject *kobj, struct kobj_attribute *attr,
                          char *buf) {
  return sprintf(buf, "%d\n", counter++);
}
