---
title: "Linux Kernel Security Hardening"
tags: [linux, kconfig, security]
draft: false
date = "2026-11-03"
---

# Linux Kernel Security Hardening

Linux Kernel 安全性相關設定

----
## MITIGATE_SPECTRE_BRANCH_HISTORY

Mitigate the risk of Spectre-BHB (Branch History Buffer) attacks through ARM64 branch history  
mitigations.

[ARM: Spectre-BHB: Speculative Target Reuse Attacks](https://developer.arm.com/-/media/Arm%20Developer%20Community/PDF/Security%20Update%2008%20March%202022/Spectre-BHB%20White%20Paper%20v1.6.pdf)

Spectre-BHB, Branch History Buffer: 分支歷史注入；處理器利用分支歷史緩衝區預測程式分支走向。攻擊者在  
userland 操作 BHB 狀態繞過 Spectre-v2 硬體級別防護，利用 speculative execution 結合未經特權的 eBPF 功能  
讓 Kernel 洩漏核心記憶體資料。

### Validation

```bash
$ zcat /proc/config.gz | grep -E 'CONFIG_(MITIGATE_SPECTRE_BRANCH_HISTORY|ARM64)='
CONFIG_ARM64=y
CONFIG_MITIGATE_SPECTRE_BRANCH_HISTORY=y

$ dmesg | grep -iE 'spectre|BHB|branch history'
[    0.000000] CPU features: detected: Spectre-v4
[    0.000000] CPU features: detected: Spectre-BHB
```

----
## KASLR

KASLR, Kernel Address Space Layout Randomization 核心位址空間佈局隨機化  
+ RANDOMIZE_BASE
+ RANDOMIZE_MODULE_REGION_FULL

Purpose: Hides the location of kernel code by randomizing where the kernel is loaded into memory at  
boot time.

### Validation

```bash
$ zcat /proc/config.gz | grep -E 'CONFIG_(RANDOMIZE_BASE|RANDOMIZE_MODULE_REGION_FULL)='
CONFIG_RANDOMIZE_BASE=y
CONFIG_RANDOMIZE_MODULE_REGION_FULL=y

# 'nokaslr' should not be set
$ cat /proc/cmdline

# the addresses should be different after a reboot
$ cat /proc/kallsyms | head -n 3 | grep _stext

# Alternatively observe the module load address
$ cat /proc/modules | grep foo
```

----
## Kstack Randomization

Kstack Randomization, Kernel Stack Offset Randomization  
+ RANDOMIZE_KSTACK_OFFSET

Purpose: Hides the location of stack data by adding a small, random offset to the kernel stack  
pointer on every system call.

----
## DEBUG_WX

Purpose: Check the kernel memory mappings for any writable and executable (w+x) regions to ensure  
that no memory pages have both write and execute permissions.

### Validation

```bash
$ dmesg | grep -iE 'Checked W\+X mappings|W\+X pages'
Checked W+X mappings: passed, no W+X pages found

# OPTIONAL: check kernel mapping manually
$ mount -t debugfs none /sys/kernel/debug
$ cat /sys/kernel/debug/kernel_page_tables
```

----
## ARM64_SW_TTBR0_PAN

PAN, Privileged Access Never

Purpose: This switches `TTBR0_EL1` when entering and exiting the kernel to prevent the kernel from  
accidentally accessing user-space memory during execution.

This will increase syscall overhead.

```bash
# syscall test
for test in null read write; do
    lat_syscall -N 1000 "$test"
done

# context switch test
lat_ctx -N 100 2 4 8
```

----
## Lockdown Linux Security Module

LSM, Lockdown Linux Security Module  
+ SECURITY_LOCKDOWN_LSM
+ SECURITY_LOCKDOWN_LSM_EARLY
+ LOCK_DOWN_KERNEL_FORCE_INTEGRITY
+ LOCK_DOWN_KERNEL_FORCE_CONFIDENTIALITY

Purpose: Enforce Linux Kernel Lockdown in confidentiality/intefrity mode, restricting operations  
that could expose kernel memory or sensitive information, such as reading certain kernel memory  
regions, loading untrusted kernel modules, and accessing certain debugging interfaces.