#ifndef SIMPLE_SENSOR_CACHE_COMMON
#define SIMPLE_SENSOR_CACHE_COMMON

struct simple_sensor_cache_data {
  struct gpio_desc *gpio;
  struct platform_device *pdev;
};

#define LKM_PRINT_ERR(pdev, fmt, ...)                                          \
  dev_err(&pdev->dev, "[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__)

#endif
