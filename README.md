Kernel module to create symlinks within /sys/platform to try tegra driver

License: GPL

Use
1. Clone kernel
```
git clone https://chromium.googlesource.com/chromiumos/third_party/kernel_next -b chromeos-tegra-3.10
```
2. Clone nvsymlink module
```
git clone https://github.com/marcino239/nvsymlink
```
3. Make module
```
cd kernel-next
make M=../nvsymlink
```
4. Unlock chrome module insertion
```
sudo ../nvsymlink/unlock_mods
```
5. Insert module
```
sudo insmod ../nvsymlink/nvsymlink.ko
```
6. Check if link is created
```
ls -l /sys/devices/platform/
```
