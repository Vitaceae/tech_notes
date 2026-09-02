---
title: "Linux systemd"
tags: [linux, systemd]
draft: false
---

# Linux systemd

----
## Set up Control Group

檢查 cgroup 路徑與詳細參數

```bash
$ systemctl show <service_name> -p ControlGroup
```

### 設定 CPU Quota

+ `CPUWeight=`: 分配特定服務的優先權，範圍 1-10000，預設 100。  
+ `CPUQuota=`: 分配特定服務的 CPU 時間占用配額
+ API: set-property
+ cgroup node: system.slice → cpu → cpu.max

查看設定值

```bash
# 檢查 service_name 的 quota 設定值
$ cat /sys/fs/cgroup/system.slice/<service_name>/cpu.max

# 檢查 systemd 服務的 cgroup 設定
$ systemctl show <service_name> -p CPUQuota,CPUQuotaPerSecUSec

# 追蹤即時 CPU 使用量
$ sudo systemd-cgtop
```

設定 cgroup

```bash
# 立即生效
$ sudo systemctl set-property foo.service CPUQuota=20%
# alternative
$ cat /sys/fs/cgroup/system.slice/foo.service/cpu.max
$ echo "20000 100000" | sudo tee /sys/fs/cgroup/system.slice/foo.service/cpu.max

# 取消限制(擇一)
$ sudo systemctl set-property foo.service CPUQuota=
$ echo "max 100000" | sudo tee /sys/fs/cgroup/system.slice/foo.service/cpu.max
```