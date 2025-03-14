# am2303

A Linux kernel module that implements a custom GPIO driver for the AM2303 sensor with caching. It offers:

- **One-wire communication** with the AM2303 sensor.
- **Sysfs interfaces** to read temperature and humidity values.
- A **built-in caching mechanism** to ensure sensor data is polled at most once every 2 seconds.

Sensor can be aquired from here:
- https://botland.store/multifunctional-sensors/2637-temperature-and-humidity-sensor-dht22-am2302-module-cables-5904422372712.html

## Building

1. Ensure you have the kernel headers and build tools installed:
   ```bash
   sudo apt-get install build-essential linux-headers-$(uname -r)
   ```
2. Build the module by running:
   ```bash
   make
   ```

## Usage

Connect sensor's data pin to 15'th GPIO of raspberry pi 5. And proceed with instruction.

### Device Tree Overlay (for Raspberry Pi 5)

Add the following snippet to `arch/arm64/boot/dts/broadcom/bcm2712-rpi-5-b.dts` after the `leds: leds` section:

```dts
    am2303_device {
        compatible = "raspberrypi,am2303_device";
        data-gpios = <&rp1_gpio 15 0>;
        status = "okay";
    };
```

After modifying the DTS file, recompile and update your boot partition. On boot, verify that the custom GPIO device is recognized by checking:

```bash
find /proc/device-tree/ | grep am2303
```

### Linux Kernel Module

- **Load the module:**
  ```bash
  sudo insmod am2303_driver.ko
  ```
  
- **Access sensor data via sysfs:**
  ```bash
  cat /sys/devices/platform/am2303_device/temperature
  cat /sys/devices/platform/am2303_device/humidity
  ```
  
- **Unload the module:**
  ```bash
  sudo rmmod am2303_driver
  ```

## TO-DO

There is still some stuff to improve:
- specify pin dynamically via LKM parameter
- use threaded hanlders instead of delays and sleeps. Threaded handler respond to interrupts so we wouldn't need sleeping having basically interrupts handlers.

## License

Dual MIT/GPL
