---
title: "C/C++ Programming - C Preprocessing"
tags: [programming, c]
draft: false
date = "2026-11-26"
---

# C Preprocessing

----
## Show macro content

編譯過程中展開巨集內容方便除錯。

```c
#define STR_HELPER(x)   #x
#define STR(x)          STR_HELPER(x)
#pragma message "Content of CERTAIN_MACRO: " STR(CERTAIN_MACRO)
```

----
## Expand macro

1. 替換程式碼中透過 #define 定義的巨集或常數成實際內容。
2. 複製所有透過 #include 引入的 header file 內容並展開到該行位置。
3. 依據 #ifdef、#ifndef、#if 等條件編譯指令保留或刪除對應的程式碼區塊。
4. 刪除程式碼中的所有 `//` 和 `/* ... */` 註解。
5. 加入行號和檔名標記以便發生錯誤時指出原始碼位置。

```bash
g++ -E xxx.c
```

----
## __has_include

分別於 C++17 和 C23 列入標準。Linux Kernel 現在(2026/11)仍以 C11 為主。若 backport 新功能到到舊版核心，僅  
比對版本號 `LINUX_VERSION_CODE >= KERNEL_VERSION(x, y, z)` 可能出現相容性錯誤，判斷是否引入特定 header  
file 可以解決不同版本間的移植問題。

```c
#if defined(__has_include)
#  if __has_include(<linux/foo_new.h>)
#    include <linux/foo_new.h>
#  else
#    include <linux/foo_old.h>
#  endif
#endif
```