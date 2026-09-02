---
title: "Linux Control Groups"
tags: [linux, cgroup]
draft: false
---

# Linux Control Groups

+ cgroup v1: multiple independent resource trees
    + Ubuntu 16.04/18.04, Kubernetes 1.22 以下版本
+ cgroup v2: shares a single tree across all resources
    + Ubuntu 20.04 以上版本

不紀錄 cgroup v1 設定與使用方式(phase-out)

----
## Limit CPU Usage

+ cpu.max
+ cpuset

列出所有包含 CPU Quota 設定值的 cgroup

```bash
find /sys/fs/cgroup -name "cpu.max" | while read -r cgroup_path; do
    echo "CPU Max: $(cat "$cgroup_path") | $(dirname "$cgroup_path")"
done | column -t -s '|'
```

----
## Operations

判斷 cgroup 版本

```bash
# tmpfs: cgroup v1; cgroup2fs: cgroup v2
$ stat -fc %T /sys/fs/cgroup
```

檢查歸類到 service 群組的所有行程

```bash
$ cat /sys/fs/cgroup/<service>/cgroup.procs
```

加入當前 shell 的 PID 到 service 的 cgroup 群組

```bash
$ echo $$ > /sys/fs/cgroup/<service>/cgroup.procs
```