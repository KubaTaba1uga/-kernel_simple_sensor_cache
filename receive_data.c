#include <linux/delay.h>
#include <linux/dev_printk.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>

#include "common.h"

int am2303_receive_data(struct am2303_data *data)
{
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
				      "Sensor is not pulling up voltage: waits=%i, i=%i\n",
				      waits, i);
			return 1;
		}
		// Now we need to receive one bit, if voltage was high for more than 70us it
		// is 1 if shorter it is 0
		waits = 0;
		while (gpiod_get_value(data->gpio) && ++waits < 100) {
			udelay(10);
		}

		if (2 <= waits && waits <= 4) {	// If between 20us and 40us it's 0
			result[i] = 0;
		} else if (6 <= waits && waits <= 8) {	// If between 60us and 80us it's 1
			result[i] = 1;
		} else {	// If longer than 80us it's unrecognized value
			result[i] = -1;
		}
	}

	// Once we have all bits we need to decode values
	u8 humidity_number;
	u8 humidity_fraction;
	u8 temp_number;
	u8 temp_fraction;
	u8 checksum;

	for (int i = 0; i < 8; i++) {
		humidity_number = (humidity_number << 1) | result[i];
	}

	for (int i = 8; i < 16; i++) {
		humidity_fraction = (humidity_fraction << 1) | result[i];
	}

	for (int i = 16; i < 24; i++) {
		temp_number = (temp_number << 1) | result[i];
	}

	for (int i = 24; i < 32; i++) {
		temp_fraction = (temp_fraction << 1) | result[i];
	}

	for (int i = 32; i < 40; i++) {
		checksum = (checksum << 1) | result[i];
	}

	u8 expected_checksum =
	    (humidity_number + humidity_fraction + temp_number + temp_fraction) & 0xFF;

	if (checksum != expected_checksum) {
		LKM_PRINT_ERR(data->pdev,
			      "Checksum is not matching: expected=%d vs actual=%d\n",
			      expected_checksum, checksum);
		return 1;
	}

	data->humidity = (humidity_number << 8) | humidity_fraction;
	data->temprature = (temp_number << 8) | temp_fraction;
	data->checksum = checksum;

	return 0;
};
