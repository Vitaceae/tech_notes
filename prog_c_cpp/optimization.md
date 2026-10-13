---
title: "C/C++ Programming - Optimization"
tags: [programming, c, cpp]
draft: false
date = "2026-10-13"
---

# Optimization

程式最佳化技巧；部分或許過時，不適用現代架構。

Mehtods that may be deprecated to optimize C program.

----
## Arithmetic 數值運算

+ data dependencies
    + avoid waiting for results
    + use multiple variables to get partial results instead of using a single one
+ multiplication
    + time cost ~ 3 times more than addition
    + use `shift` for (2^n)
    + use `shift` and addition for (2^n - 1)
    + use `lea`, Load Effective Address
+ division
    + use `shift` for (2^n)

----
## Test an even number 測試偶數

```c
num % 2 == 0
num & 0x01 == 0
```

----
## for loop 迴圈最佳化

+ inline assembly
+ use unsigned int instead of int
+ mathematical formula 使用數學式
+ Look-up Table 查表法
+ Compiler Optimization 編譯器最佳化如 'gcc -o3'
+ hardware acceleration 硬體加速

### multi-core 多核運算

+ OpenMP
+ POSIX Threads

```c
/* OpenMP */

//omp_get_num_procs()   //get core count
//omp_get_num_threads() //get thread count

#pragma omp parallel for
for (i = 0; i <= 100; ++i) {/* actions */}

//equivalent
#pragma omp parallel
{
    #pragma omp for
    for (int i = 0; i < 100; ++ i) {/* actions */}
}
```

### Loop Unwinding 拆解迴圈

```c
    //only 10 loops here
    for(i = 1; i <= n; i += 10) {
        s += i;
        s += i + 1;
        s += i + 2;
        s += i + 3;
        s += i + 4;
        s += i + 5;
        s += i + 6;
        s += i + 7;
        s += i + 8;
        s += i + 9;
    }
```

### (C++) Template Meta Programming

將 runtime 運算提前到 compile time。

```cpp
template <unsigned n>
struct foo {
    enum {value = n + foo<n-1>::value};
};

template<>
struct foo<0> {
    enum {value = 0};
};
```

----
## Conceptual but not practical 概念說明

實際可能沒有差別。

### XOR Swap

+ 僅透過 XOR 互換數值
+ XOR 目標值若與輸入值相同, 其結果為 0: XOR_SWAP(x, x) 結果為 0
+ **NOT portable**
+ 對於現代 CPU 此法效率較差: 指令相依(輸出依賴之前輸入結果), 無法透過 pipeline 平行加速
+ XOR properties
    + involution or self-inverse 反元素: X ^ X = 0 
    + commutative 交換性: A ^ B = B ^ A
    + associative 結合性 (A ^ B) ^ C = A ^ (B ^ C)
    + 單位元 A ^ 0 = A

XOR Operation:  
| Value x | Value y | XOR result|
|:-------:|:-------:|:---------:|
| 0       | 0       | 0         |
| 0       | 1       | 1         |
| 1       | 0       | 1         |
| 1       | 1       | 0         |

```c
/*
1. X = X ^ Y
2. Y = X ^ Y = (X ^ Y) ^ Y = X ^ (Y ^ Y) = X ^ 0 = X
3. X = X ^ Y = (X ^ Y) ^ X = (Y ^ X) ^ X = Y
*/
#define XOR_SWAP(x, y)                  ((x) ^= (y), (y) ^= (x), (x) ^= (y))
```