---
title: "C/C++ Programming - C Arithmetics"
tags: [programming, c]
draft: false
date = "2026-10-27"
---

# C Arithmetics

算術運算相關

----
## Conditionals with Omitted Operands

三元運算省略表達式；GNU C Extension； 非零

```c
x ? : y

# equivalent to
x ? x : y
```

Side Effect: 表達式 x 若包含遞增/遞減運算，回傳值可能被計算兩次。

ref: [GCC: Conditionals with Omitted Operands](https://gcc.gnu.org/onlinedocs/gcc/Conditionals.html)

----
## Addition using XOR and AND Operations

1. Sum (XOR): `a ^ b` 保留加總後所有未進位的位元。
2. Carry (AND + Shift): `a & b << 1` 計算加總後進位的位元。
3. Full Addition: 重複步驟 1 和 2 直到 Carry 為 0。

python 範例:

```python
def add_without_plus(a, b):
    while b != 0:
        # XOR 不包含進位的加總
        sum_without_carry = a ^ b
        # AND 並左移表示進位
        carry = (a & b) << 1
        # 迭代: 下一次的初值為不包含進位的加總
        a = sum_without_carry
        # 迭代: 下一次的加數為計算出的進位
        b = carry
    return a

print(add_without_plus(5, 3))
```

----
## Bitmask's Left Shift Count 遮罩左移位元數

計算 Trailing Zeros

GCC/Clang: 使用 CPU 指令如 TZCNT 或 BSF。

```c
int shift = __builtin_ctz(mask); // Count Trailing Zeros
```

或位移直到最右側位元值為 1。

```c
int shift = 0;
// 2's complement
unsigned int low_bit = mask & -mask; 

while (((low_bit >> shift) & 1) == 0) {
    shift++;
}
```