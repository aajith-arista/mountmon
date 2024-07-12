<h1>mountmon</h1>

<h2>>Summary</h2>
Mount monitor binary using https://cdn.kernel.org/pub/linux/utils/util-linux/v2.33/libmount-docs/libmount-Monitor.html

<h2>>Build</h2>
make builds the binary

glibc.so provided for LD_PRELOAD because we use  `getgrnam_r`, `getpwnam_r` and `getpwuid_r` and ld warns:
```
warning: Using '<func>' in statically linked applications requires at runtime the shared libraries from the glibc version used for linking`
```
All libs are statically linked otherwise

