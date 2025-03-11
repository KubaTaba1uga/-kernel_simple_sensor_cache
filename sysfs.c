#include <linux/device.h>
#include <linux/ktime.h>

#include "sysfs.h"
#include "common.h"
#include "linux/mutex.h"
#include "receive_data.h"
#include "set_up_communication.h"

static ssize_t show_temp(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t show_humid(struct device *dev, struct device_attribute *attr, char *buf);
static int receive_data(struct am2303_data *data);

static DEVICE_ATTR(temperature, 0440, show_temp, NULL);
static DEVICE_ATTR(humidity, 0440, show_humid, NULL);
static DEFINE_MUTEX(recv_data_lock);

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

	mutex_init(&recv_data_lock);

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
	int err;

	data = dev_get_drvdata(dev);

	err = receive_data(data);
	if (err) {
		LKM_PRINT_ERR(data->pdev, "Unable to receive temprature from sensor");
		return -1;
	}

	return snprintf(buf, 32, "%d\n", data->temprature);
}

static ssize_t show_humid(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct am2303_data *data;
	int err;

	data = dev_get_drvdata(dev);

	err = receive_data(data);
	if (err) {
		LKM_PRINT_ERR(data->pdev, "Unable to receive humidity from sensor");
		return -1;
	}

	return snprintf(buf, 32, "%d\n", data->humidity);
}

static int receive_data(struct am2303_data *data)
{
	struct timespec64 now;
	int err;

	mutex_lock(&recv_data_lock);

	ktime_get_ts64(&now);

	// We need to wait at least 2 seconds between data collections
	if (now.tv_sec - data->last_receive.tv_sec <= 2) {
		err = 0;
		goto cleanup;
	}

	err = am2303_set_up_communication(data);
	if (err) {
		LKM_PRINT_ERR(data->pdev, "Unable to set up communication with sensor");
		goto cleanup;
	}

	err = am2303_receive_data(data);
	if (err) {
		LKM_PRINT_ERR(data->pdev, "Unable to receive data from sensor");
		goto cleanup;
	}

	data->last_receive = now;

	err = 0;

 cleanup:
	mutex_unlock(&recv_data_lock);

	return err;
}
