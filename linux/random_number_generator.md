---
title: "Linux Random Number Generator"
tags: [linux, rng]
draft: false
date = "2026-11-03"
---

# Linux Random Number Generator

RNG, Random Number Generator

----
## Validation

CHECK: Kernel configurations are enabled and TPM device exists

```bash
$ zcat /proc/config.gz 2>/dev/null | grep -E 'CONFIG_(HW_RANDOM|HW_RANDOM_TPM|TCG_TPM)='
CONFIG_HW_RANDOM=y
# if TPM supported
CONFIG_TCG_TPM=y
CONFIG_HW_RANDOM_TPM=y

# OPTIONAL: check if tpm dev node exists
$ ls -l /sys/class/tpm/ 2>/dev/null
$ ls -l /dev/tpm* 2>/dev/null

# Check kernel messages related to TPM and RNG
$ dmesg | grep -Ei 'tpm|hwrng|hw.random|random number'
[    1.234567] foo-hwrng <base_addr>.rng: rng probed
```

CHECK: RNG driver is operational

```bash
# Check the registered RNG drivers
$ cat /sys/class/misc/hw_random/rng_available 2>/dev/null
foo-hwrng

# Check which RNG is currently used by /dev/hwrng
$ cat /sys/class/misc/hw_random/rng_current 2>/dev/null
foo-hwrng

# Verify the device node exists
$ ls -l /dev/hwrng 2>/dev/null
```

CHECK: The system can correctly use the random number source

```bash
$ dd if=/dev/hwrng of=/tmp/tpm-rng.bin bs=64 count=16
$ rc=$? && echo "dd exit code: $rc"
dd exit code: 0

$ ls -l /tmp/tpm-rng.bin 2>/dev/null
$ od -An -tx1 -N64 /tmp/tpm-rng.bin
$ sha256sum /tmp/tpm-rng.bin
```

----
## Test randomness

使用 `netpbm` 和 `rng-tools` 將隨機數輸出為圖像，視覺檢驗是否有規律。

```bash
$ sudo apt install netpbm rng-tools

# Use /dev/urandom as a fallback if /dev/hwrng is unavailable.
$ sudo cat /dev/hwrng | rawtoppm -rgb 256 256 | pnmtopng > random$(date +%Y%m%d%H%M%S).png
```

使用 `rngtest` 簡單檢驗:

```bash
$ sudo cat /dev/hwrng | rngtest -c 1000
rngtest 2-unofficial-mt.14
```

使用 `dieharder` 嚴格檢驗:

(TODO)

----
## RNG on WSL

### virtio-rng (FAILED)

啟用 virtio-rng 驅動，Hyper-V 透過虛擬化通道將隨機數傳給 WSL。

VM 設定似乎不適用 WSL。

+ [Windows VirtIO Drivers](https://pve.proxmox.com/wiki/Windows_VirtIO_Drivers)
+ 安裝 virtio-win-gt-x64: [Fedora download page](https://fedorapeople.org/groups/virt/virtio-win/direct-downloads/archive-virtio/virtio-win-0.1.302-1/)

嘗試開啟 WSL Config。手動更新為最新預覽版本。

```powershell
> wsl --shutdown
> wsl --update --pre-release
Checking for updates.
Updating Windows Subsystem for Linux to version: 3.0.5.
```

設定虛擬機組態

```bash
$ vi ~/.wslconfig
[wsl2]
virtio=true
virtiofs=true
hostFileSystemAccess=true
```

rng_available 依然為 none

### Use /dev/random or /dev/urandom

`/dev/random` 為 Blocking，熵不足時會阻塞等待；/dev/urandom 為 Non-blocking，利用內部加密演算法持續生成隨  
機數。

### Feed entropy pool

rng-tools 利用 CPU 硬體隨機數指令餵給熵池；haveged 利用 CPU 循環時間抖動(Loop time jitter) 生成偽隨機數填  
補熵池。haveged 生成的隨機數品質有疑慮。

```bash
$ sudo apt install haveged rng-tools

# Check the current available entropy.
$ cat /proc/sys/kernel/random/entropy_avail
```