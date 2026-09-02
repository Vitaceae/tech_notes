---
title: "Linux Profiling"
tags: [linux, profiling, perf]
draft: false
---

# Linux Profiling

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
