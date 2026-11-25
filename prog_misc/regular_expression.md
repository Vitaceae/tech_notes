---
title: "Programming - Regular Expression"
tags: [programming, regex]
draft: false
date = "2026-11-25"
---

# Regular Expression 正規表示式

----
## Examples

移除 ANSI 控制碼

```
# code pattern: \x1b\[[0-9;]*[a-zA-Z]
$ sed -E 's/\x1b\[[0-9;]*[a-zA-Z]//g
```