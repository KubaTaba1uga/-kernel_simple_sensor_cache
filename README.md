# Usage

## Prepare dts file

We need to compile device tree overlay to configure GPIO pin and use it from our kernel module. To do so follow instructions below.

Compile dtb file:
```
make dt
```

And copy it on boot partition into overlays dir:
```
$ sudo cp simple_sensor_cache.dtbo /opt/tftp/b7afe72d/overlays/
```

Now add to config.txt:
```
dtoverlay=simple_sensor_cache
```

Reboot and confirm overlay is working:
```
# find /proc/device-tree/ | grep simple
/proc/device-tree/axi/pcie@120000/rp1/gpio@d0000/simple_sensor_cache_device
/proc/device-tree/axi/pcie@120000/rp1/gpio@d0000/simple_sensor_cache_device/data-gpios
/proc/device-tree/axi/pcie@120000/rp1/gpio@d0000/simple_sensor_cache_device/compatible
/proc/device-tree/axi/pcie@120000/rp1/gpio@d0000/simple_sensor_cache_device/name
```
