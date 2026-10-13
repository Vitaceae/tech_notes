---
title: "C/C++ Programming - C Library"
tags: [programming, c, cpp]
draft: false
date = "2026-10-13"
---

# C Library

C 函式庫小抄

----
## stdio family

字串/數值轉換函數: atoi, atof, strtol, strtoll, strtod

----
## scanf()

+ suggest to use **fgets** instead
+ formatted string input 格式化輸入
    + `%[a-z]`: 輸入值範圍為 a 到 z 字元, 遇到例外則停止; 如 `%[ny]` 僅接受輸入值為 n 或 y
    + `^`: negates the succeeding characters; 例如 `%[^.]` 遇到 . 字元停止
    + ` %[^\n]%*c`: remove leading spaces; %\*c discards succeeding garbage characters

若輸入緩衝區殘留值可能造成非預期結果, 掃描前後需注意清空  
但使用 `fflush(stdin)` 清空輸入緩衝區屬於非定義行為  

輸入字串終止條件為 white space, tab 或 newline  
`scanf("%4s", buf);` 後輸入 'a bc' 結果為 'a', 可改為 `%[^\n]%*c` 處理  

可能造成 buffer overflow: Segmentation fault 的例子:  
```c
char buf[4] = {0};
scanf("%s", buf);
//user input '12345'
```

----
## fgets()

example: [clib_fgets.c](inc/clib_fgets.c)

----
## Sorting

### qsort

```c
void qsort(void *base, size_t n, size_t size, int (*cmp)(const void*, const void*));
```

example: [clib_qsort.c](inc/clib_qsort.c)

----
## Using Linker Symbols in Applications

+ 32-bit function/variable/arbitrary address
+ TI uses `_symval()`

```c
# function-equivalent(say sym_func=printf+100;)
printf("sym_func %lx\n", _symval(&sym_func));
printf("sym_func %lx\n", (unsigned long)&sym_func);

# variable-equivalent(say sym_data=.data+100;)
printf("sym_data %p\n", &sym_data);
printf("sym_data %lx\n", _symval(&sym_data));

# arbitrary
printf("abs_sym %lx\n", _symval(&sym_abs));
```