---
title: "Software Engineering - Code Formatter"
tags: [programming, se]
draft: false
date = "2026-11-06"
---

# Code Formatter

程式碼格式化與排版工具

----
## indent

+ [indent command guide](https://www.gnu.org/software/indent/manual/html_chapter/indent_1.html)

```bash
# Linux kernel style
$ indent \
    -nbad -bap -nbc -bbo -hnl -br -brs -c33 -cd33 -ncdb -ce -ci4 \
    -cli0 -d0 -di1 -nfc1 -i8 -ip0 -l80 -lp -npcs -nprs -npsl -sai \
    -saf -saw -ncs -nsc -sob -nfca -cp33 -ss -ts8 -il1 \
    $target_file
```