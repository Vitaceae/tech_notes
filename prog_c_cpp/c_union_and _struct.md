---
title: "C/C++ Programming - C Union and Struct"
tags: [programming, c]
draft: false
date = "2026-10-12"
---

# C Union and Struct

----
## union

所有成員共用記憶體空間; 最大成員決定結構大小。

```c
union foo {
    struct {
        unsinged int d1: 4;
        unsigned int d2: 2;
        unsigned int reserved: 26;
    } s;
    unsigned char c;
}

typedef union foo       foo_u
```

Initialization:  

```c
//undefined behavior
foo_u bar = {1, 2, 5};

//correct
foo_u bar;
bar.s.d1 = 1, bar.s.d2 = 2, bar.s.reserved = 5;

//certain versions allow the following
foo_u bar = {.s.d1 = 1, s.d2 = 2, .s.reserved = 5};
```

----
## struct

padding:  
+ 對齊邊界 (compiler-dependent)
+ gcc 可使用修飾詞 `__attribute__((packed))` 避免填充

相同資料型態大小相同，不會填充。

```c
struct m1 {
    char c1;
    char c2;
    char c3;
}

//sizeof(struct m1) 為 3
```

不同資料型態依預設對齊方式填充。

```c
struct m1 {
    char a;
    int b;
    char c;
};
//a(1) + pad(3) + b(4) + c(1) + pad(3) = 12

struct m2 {
    char a;
    char b;
    int c;
};
//a(1) + b(1) + pad(2) + c(4) = 8

struct m3 {
    int a;
    char b;
    char c;
};
//a(4) + b(1) + c(1) + pad(2) = 8
```

為相容舊式編譯器，建議同資料型態並列, 由大而小或由小而大進行宣告。

bit-field padding

```c
        //size equals 4
        struct s {
            unsigned a: 1;
            unsigned b: 2;
        };
        //size equals 4
        struct s1 {
            unsigned a: 1;
            int b: 1;
        };
        //ieee754; unsigned int 與 unsigned char 資料型態大小不同; padding 成 64 bits
        struct _754 {
            unsigned int mantissa: 23;
            unsigned char exponment: 8;
            unsigned char sign: 1;
        };
```