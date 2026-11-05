---
title: "Bash Programming - Options"
tags: [programming, bash]
draft: false
date = "2026-11-05"
---

# Bash Options

Bash 常用參數紀錄

----
## Useful options

```bash
# Enable option
set -<option>

# Disable option
set +<option>
```

+ -e: errexit; 任何命令返回非零值時立即退出腳本。
+ -E: errtrace; function, command substitution 以及 subshell 繼承主腳本的 ERR trap。
+ -x: xtrace; 除錯時開啟，輸出所有執行指令。
+ -u: nounset; 使用未定義變數時回報錯誤並退出。
+ -v: verebose; 輸出執行指令，若為迴圈僅輸出計算結果。不展開變數值。
+ -o pipefail: 以管道最後一個指令的 exit code 作為最終結果，無視前面指令錯誤，視為成功執行。

Example: errtrace

```bash
#!/bin/bash

error_handler() {
    echo "Error trapped. Line $1: $BASH_COMMAND"
}

# Register a signal handler for the ERR signal.
trap 'error_handler $LINENO' ERR

foo() {
    echo "enter foo()"
    ls /path/to/file/does/not/exist
    echo "exit foo()"
}

echo "without errtrace"
foo

echo "enable errtrace"
set -E
foo
```

Example: pipefail

```bash
set -euo pipefail

# 若不包含字串 bar 回傳值為 1，可能導致腳本結束(非期望行為)。
res=$(cat $FOO | grep bar)

# 找不到字串時仍然回傳 0(true)，避免 pipefail。
res=$(cat $FOO | grep bar || true)
```

----
## 重設輸入參數

```bash
set -- foo1 foo2 foo3

echo "$2"
# foo2
```