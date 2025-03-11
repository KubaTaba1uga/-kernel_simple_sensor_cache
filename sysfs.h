#ifndef AM2303_SYSFS
#define AM2303_SYSFS

#include "common.h"

int am2303_init_sysfs(struct am2303_data *data);

void am2303_destroy_sysfs(struct am2303_data *data);

#endif
