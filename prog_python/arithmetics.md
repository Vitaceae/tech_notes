---
title: "Python Programming - C Arithmetics"
tags: [programming, python]
draft: false
date = "2026-10-27"
---

# Arithmetics

算術運算相關

----
## Bitmask's Left Shift Count 遮罩左移位元數

計算 Trailing Zeros

```python
shift = (mask & -mask).bit_length() - 1

# if supported
shift = mask.trailing_zeros()
```
