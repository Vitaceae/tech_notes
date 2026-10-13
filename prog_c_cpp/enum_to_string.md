---
title: "C/C++ Programming - Enum to String"
tags: [programming, c, cpp]
draft: false
date = "2026-10-12"
---

# Enum to String

將 enum 列舉值自動轉換成字串；C/C++ 中缺少如 C#、Java 等語言原生支援方案。

----
## Use switch

直觀；但麻煩。

```c
enum ENUM_INDEX {
    IDX_0,
    IDX_1,
    IDX_2,
    IDX_MAX
};

const char *enum_to_str(ENUM_INDEX index)
{
    switch (index) {
    case IDX_0:
        return "Value 0";
    case IDX_1:
        return "Value 1";
    case IDX_2:
        return "Value 2";
    default:
        break;
    }

    return "unknown";
}
```

----
## Use One-to-One Mapping Table

同步建立一對一取值表；表單過長時比對索引值較麻煩，也可能漏定義造成非預期行為。

```c
enum ENUM_INDEX {
    IDX_0,
    IDX_1,
    IDX_2
};

static char *enum_val[] = {
    "Value 0",
    "Value 1",
    "Value 2"
};

const char *enum_to_str(ENUM_INDEX index)
{
    if (index < IDX_MAX) {
        return enum_val[index];
    }

    return "unknown";
}
```

----
## Use X Macro

建立查表陣列，改善一對一取值表的問題。

```c
#define IDX_TABLE \
    X(IDX_0, "Value 0") \
    X(IDX_1, "Value 1") \
    X(IDX_2, "Value 2") \
    X(IDX_MAX, "Value Max")

#define X(a, b)     a,
enum ENUM_INDEX {IDX_TABLE};
#undef X

const char *enum_to_str(ENUM_INDEX index)
{
#define X(a, b)     b,
    static char *enum_table[] = {IDX_TABLE};
#undef X

    if (index > IDX_MAX) {
        index = IDX_MAX;
    }

    return enum_table[index];
}
```

ref: [Wiki: X Macro](https://en.wikipedia.org/wiki/X_macro)

----
## Use X Macro and std::map

C++ 限定；索引不連續的查表法。若 enum 不從 0 開始列舉或索引不連續時可能浪費空間。std::map 使用紅黑樹實作，時
間複雜度 O(logN)。

```cpp
#define IDX_TABLE \
    X(IDX_0, = 0, "Value 0") \
    X(IDX_6, = 6, "Value 6") \
    X(IDX_15, = 15, "Value 15")

#define X(a, b, c)     a, b
enum ENUM_INDEX {IDX_TABLE};
#undef X

const char *enum_to_str(ENUM_INDEX index)
{
    static std::map<ENUM_INDEX, const char*> enum_table;
    static bool is_table_ready = false;

    if (is_table_ready) {
        return enum_table[index];
    }

#define X(a, b, c)  enum_table[a] = c;
    IDX_TABLE
#undef X

    is_table_ready = true;

    return enum_table[index];
}
```
