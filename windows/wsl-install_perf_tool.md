---
title: "WSL - Install Perf Tool"
tags: [windows, wsl, perf]
draft: false
---

# WSL - Install Perf Tool

WSL2 版本 v0.50.2 後支援 Hardware Performance Counters，可用於分析的效能事件較完整。

修改 `%USERPROFILE%\.wslconfig`

```bash
[wsl2]
hardwarePerformanceCounters=true
```

----
## (FAILED)Install via apt Command

```bash
$ sudo apt update
$ sudo apt install flex bison libelf-dev libaudit-dev libslang2-dev libperl-dev libiberty-dev libunwind-dev python3-dev
$ sudo apt install linux-tools-common linux-tools-generic

$ sudo apt install linux-tools-$(uname -r)

E: Unable to locate package linux-tools-6.18.33.2-microsoft-standard-WSL2
E: Couldn't find any package by glob 'linux-tools-6.18.33.2-microsoft-standard-WSL2'

$ sudo apt install linux-tools-standard-WSL2

E: Unable to locate package linux-tools-standard-WSL2
```

----
## Manually Build and Install

Ref. [Install perf on WSL 2](https://gist.github.com/abel0b/b1881e41b9e1c4b16d84e5e083c38a13)

```bash
$ apt install flex bison libtraceevent-dev
$ git clone git@github.com:microsoft/WSL2-Linux-Kernel.git --depth 1
$ cd WSL2-Linux-Kernel/tools/perf
$ make -j8
$ sudo cp perf /usr/local/bin
```