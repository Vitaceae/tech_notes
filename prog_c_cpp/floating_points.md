---
title: "C/C++ Programming - Floating Points"
tags: [programming, c, cpp]
draft: false
date = "2026-10-13"
---

# Floating Points

C 語言浮點數規範

|format     |precision  |bytes|range                  |Significant Figures|
|:----------|:----------|:----|:----------------------|:------------------|
|float      |single     |4    |±3.4e-38~±3.4e+38      |7-1                |
|double     |double     |8    |±1.79E-308 ~ ±1.79E+308|16-1               |
|long double|long-double|16   |±1.79E-308 ~ ±1.79E+308|19-1               |

+ sign(S) 符號數
    + 1 bit: 0=positive, 1=negative
+ exponent(E) 指數
    + 8 bits for biasing
        + exponents are signed values to represent both tiny and huge values
        + 2's complement makes comparison harder
        + add the bias when storing(unsigned); subtract the bias when interpreting
+ mantissa(M) 尾數
    + 23 bits
+ NaN, Not a Number
    + E: all 1
    + M: not all 0
    + Quiet NaNs, QNaNs, 不確定值(0/0; Inf x 0)
    + Signaling NaNs, SNaNs, 無效操作
+ Zero
    + E: all 0
    + M: all 0
+ Infinite
    + E: all 1
    + M: all 0
+ subnormal, denormal, denormalized number  
    ```text
      -Inf    -Normalized   -Denormal    +Denormal   +Normalized    +Inf
        ├───┼─────────────┼────────────┼───────────┼─────────────┼───┤
    NaN                                ^                               NaN
                                     -0 +0
    ```
    + E: all 0
    + M: not all 0
    + 超過規格化表示範圍, 即 UFL, UnderFlow Level, 最小可表示數
    + ALU 僅能計算正規數, 非正規數一般觸發硬體中斷處理

----
## Example of a 32-bit float

```text
     S   E                M
    ├──┼────────────────┼──────────────────────────────────────────────┤
    31 30               22                                             0
    1b 8b               23b

NaN, Not a Number
     x  1 1 1 1 1 1 1 1  n n n n n n n n n n n n n n n n n n n n n n n
Zero
     x  0 0 0 0 0 0 0 0  0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
Infinite
     x  1 1 1 1 1 1 1 1  0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
subnormal
     x  0 0 0 0 0 0 0 0  n n n n n n n n n n n n n n n n n n n n n n n
convert 7.25 from decimal to binary:
     0  1 0 0 0 0 0 0 1  1 1 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

     1. 7d = 111b
     2. 0.25d = 0.01b:
            0.25 * 2 = 0.5  ...no carry, 1st digit is 0
            0.5 * 2 = 1     ...with carry, 2nd digit is 1
    3. 7.25d = 111.01b
    4. S=0 for positive
    5. 111.01 shifts right by 2 places: 1.1101(IEEE standard: start by 1)
            E = (bias 127) + (2) = 129d = 10000001b
    6. M = 1101000...b
convert from binary expression to dec: 
    S = 0, +1d
    E = 10000001b=129d, 129 - 127 = 2d
    M = 0.1101b + 1b = 1.1101b = 1d + 0.5d + 0.25d + 0.0625d = 1.8125d
    S * M * 2^E = +1 * 1.8125 * 4 = 7.25d
```