#include <linux/device.h>
#include <linux/ktime.h>

#include "common.h"
#include "linux/mutex.h"
#include "receive_data.h"
#include "set_up_communication.h"
#include "sysfs.h"

static ssize_t show_temp(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t show_humid(struct device *dev, struct device_attribute *attr, char *buf);
static int receive_data(struct am2303_data *data);

static DEVICE_ATTR(temperature, 0440, show_temp, NULL);
static DEVICE_ATTR(humidity, 0440, show_humid, NULL);

int am2303_init_sysfs(struct am2303_data *data)
{
	int err;

	data->last_receive.tv_sec = 0;
	data->last_receive.tv_nsec = 0;

	err = device_create_file(&data->pdev->dev, &dev_attr_temperature);
	if (err) {
		return err;
	}

	err = device_create_file(&data->pdev->dev, &dev_attr_humidity);
	if (err) {
		return err;
	}

	mutex_init(&data->access_mutex);

	return 0;
};

void am2303_destroy_sysfs(struct am2303_data *data)
{
	device_remove_file(&data->pdev->dev, &dev_attr_temperature);
	device_remove_file(&data->pdev->dev, &dev_attr_humidity);
};

static ssize_t show_temp(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct am2303_data *data;
	int err = 0;

	data = dev_get_drvdata(dev);

	mutex_lock(&data->access_mutex);

	err = receive_data(data);
	if (err) {
		LKM_PRINT_ERR(data->pdev, "Unable to receive temprature from sensor\n");
		goto cleanup;
	}

	err = snprintf(buf, 32, "%d\n", data->temprature);
	if (err < 0) {
		LKM_PRINT_ERR(data->pdev, "Unable to write temprature into sysfs buffer\n");
		goto cleanup;
	}

 cleanup:
	mutex_unlock(&data->access_mutex);

	return err;
}

static ssize_t show_humid(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct am2303_data *data;
	int err = 0;

	data = dev_get_drvdata(dev);

	mutex_lock(&data->access_mutex);

	err = receive_data(data);
	if (err) {
		LKM_PRINT_ERR(data->pdev, "Unable to receive humidity from sensor\n");
		goto cleanup;
	}

	err = snprintf(buf, 32, "%d\n", data->humidity);
	if (err < 0) {
		LKM_PRINT_ERR(data->pdev, "Unable to write humidity into sysfs buffer\n");
		goto cleanup;
	}

 cleanup:
	mutex_unlock(&data->access_mutex);

	return err;
}

static int receive_data(struct am2303_data *data)
{
	struct timespec64 now;
	int err;

	ktime_get_ts64(&now);

	// We need to wait at least 2 seconds between data collections
	if (now.tv_sec - data->last_receive.tv_sec <= 2) {
		return 0;
	}

	err = am2303_set_up_communication(data);
	if (err) {
		LKM_PRINT_ERR(data->pdev, "Unable to set up communication with sensor");
		return err;
	}

	err = am2303_receive_data(data);
	if (err) {
		LKM_PRINT_ERR(data->pdev, "Unable to receive data from sensor");
		return err;
	}

	data->last_receive = now;

	return 0;
}
