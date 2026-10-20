---
title: "Linux Profiling"
tags: [linux, profiling, perf]
draft: false
date = "2026-10-20"
---

# Linux Profiling

----
## strace

an example script to start an application foo and save the trace to $LOG_FILE.

```bash
# append a timestamp to the log file
LOG_FILE="/tmp/foo_$(date +%Y%m%m_%H%M%S).log"

# start the app
# -f: follow forks
# -tt: timestamp with ms
# -T: display syscall time
# -e trace=...: trace filter; 只追蹤指定的 syscall
$ exec strace -f -tt -T -e trace=... -o "$LOG_FILE" foo
```

----
## Benchmark

測試核心組態修改前後的系統性能差異  
+ [perf script](./inc/profiling/benchmark_hardening.sh): 取得性能數據  
  `./benchmark_hardening > baseline.log 2>&1`  
  `./benchmark_hardening > hardened.log 2>&1`
+ [parser](./inc/profiling/benchmark_hardening_log_parser.py): 分析修改前後日誌，比較差異。  
  `python benchmark_hardening_log_parser baseline.log hardened.log`
+ [perf script(sys)](./inc/profiling/benchmark_hardening_sys.sh): 粗略比較系統負載

