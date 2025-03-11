#ifndef SIMPLE_SENSOR_CACHE_COMMON
#define SIMPLE_SENSOR_CACHE_COMMON

struct simple_sensor_cache_data {
  struct gpio_desc *gpio;
  struct platform_device *pdev;
};

#endif
