Kernel module to create symlinks within /sys/platform to try tegra driver

License: GPL

Use
*  Clone kernel
```
git clone https://chromium.googlesource.com/chromiumos/third_party/kernel_next -b chromeos-tegra-3.10
```
*  Clone nvsymlink module
```
git clone https://github.com/marcino239/nvsymlink
```
*  Make module
```
cd kernel-next
make M=../nvsymlink
```
*  Unlock chrome module insertion
```
sudo ../nvsymlink/unlock_mods
```
*  Insert module
```
sudo insmod ../nvsymlink/nvsymlink.ko
```
*  Check if link is created
```
ls -l /sys/devices/platform/
```
