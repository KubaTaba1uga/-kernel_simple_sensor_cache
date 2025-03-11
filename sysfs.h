#ifndef SIMPLE_SENSOR_CACHE_SYSFS
#define SIMPLE_SENSOR_CACHE_SYSFS

#include "common.h"

int simple_sensor_cache_init_sysfs(struct simple_sensor_cache_data *data);

void simple_sensor_cache_destroy_sysfs(struct simple_sensor_cache_data *data);

#endif
