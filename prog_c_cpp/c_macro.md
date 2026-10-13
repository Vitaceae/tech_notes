---
title: "C/C++ Programming - C Macro"
tags: [programming, c, cpp]
draft: false
date = "2026-10-13"
---

# C Macro

----
## Prevent from multiple inclusion 避免重複引入

```c
#ifndef COMDEF_H 
#define COMDEF_H 

//contents

#endif 
```

----
## 比較大小

```c
#define MAX(x, y)   (((x) > (y)) ? (x) : (y)) 
#define MIN(x, y)   (((x) < (y)) ? (x) : (y)) 
```

----
## Get struc member 取得結構成員資訊

```c
#define offset_of(type, field)  ((dword)&(( type *)0)->field)
#define length_of(type, field)  sizeof(((type *)0)->field) 
```

----
## Round to Nearest Integer in base 8 進位至最接近的八進位數

```c
#define ROUND_OCTAL(x)  ((((x) + 7) / 8 ) * 8)
```

----
## Capitalize a Letter 轉換大小寫字母

```c
#define UPPER_CASE(c)   (((c) >= 'a' && (c) <= 'z') ? ((c) - 0x20) : (c)) 
```

----
## Prevent from Overflow

```c
#define INC_SAT(val)    (val = ((val)+1 > (val)) ? (val)+1 : (val)) 
```

----
## Get Array Element Count

```c
#define ARR_SIZE(x)     (sizeof((x)) / sizeof((x[0])))
```

----
## Unsigned Integer Modulo 2^n 計算 2^n 的同餘

```c
#define MOD_POW_OF_2(val, divisor)  ((dword)(val) & (dword)((divisor) - 1)) 
```

----
## 合併參數或字串

1. `##` 用於連接不同參數，產生新參數名
2. `#` 將參數轉換為字串

```c
#define TO_STR(s)       #s
#define CALC_EXP(a, b)  int(a##e##b)

printf(TO_STR(test)); // output "test" 
printf("%d\n", CALC_EXP(5, 3)); //int(5e3), i.e., 5000
```

巢狀展開；若定義中包含 `#` 或 `##` 不會重複展開，需要多嵌套一層轉換

```c
#define VAL         (4) 
#define CALC(a, b)  (a * b)

//展開為 printf("%d * %d = %d\n", (4), (4), ((4) * (4)));
printf("%d * %d = %d\n", VAL, VAL, CALC(VAL, VAL));

#define TO_STR(s)       #s 
#define CALC_EXP(a, b)  int(a##e##b)

//展開為 printf("INT_MAX=%s\n", "INT_MAX"); 非預期
printf("INT_MAX=%s\n", TO_STR(INT_MAX));
//展開為 printf("%d\n", int(VALeVAL)); 非預期; compile error
printf("%d\n", CALC_EXP(VAL, VAL));

//解決方法: 加入轉換層
#define _TO_STR(s)          #s
#define TO_STR(s)           _TO_STR(s)
#define _CALC_EXP(a, b)     int(a##e##b)
#define CALC_EXP(a, b)      _CALC_EXP(a,b)
```

C++ 合併匿名變數(anonymous variable)

```c
#define __ANON(type, var, line)     type var##line 
#define _ANON(type, line)           __ANON(type, _anonymous, line)
#define ANON(type)                  _ANON(type, __LINE__) 
```

填充結構體

```c
#define ADD_ID(x)       {x, #x}

enum MSG_ID {DATA1, DATA2};

typedef struct my_message_s { 
    MSG_ID id;
    const char *str;
} my_msg_t;

//等同 my_msg_t m[] = {{DATA1, "DATA1"}, {DATA1, "DATA1"}};
my_msg_t m[] = {ADD_ID(DATA1), ADD_ID(DATA2)}; 
```

記憶所在檔名

```c
#define _THIS_FILE(f)   #f 
#define THIS_FILE(f)    _THIS_FILE(f)

static char file_name[] = THIS_FILE(__FILE__);
```
