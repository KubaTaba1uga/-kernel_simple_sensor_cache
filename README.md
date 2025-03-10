# Usage

## Prepare dts file

Add to arch/arm64/boot/dts/broadcom/bcm2712-rpi-5-b.dts after `leds: leds` section
```
    simple_sensor_cache_device {
        compatible = "raspberrypi,simple_sensor_cache_device";
        data-gpios = <&rp1_gpio 15 0>;
        status = "okay";
    };
```

Recompile dtses and upload to your boot partition. Once they are there boot up rpi5 and confirm that our custom gpio is recognized as device:
```
# find /proc/device-tree/ | grep simple
/proc/device-tree/simple_sensor_cache_device
/proc/device-tree/simple_sensor_cache_device/data-gpios
/proc/device-tree/simple_sensor_cache_device/compatible
/proc/device-tree/simple_sensor_cache_device/status
/proc/device-tree/simple_sensor_cache_device/nam
```
