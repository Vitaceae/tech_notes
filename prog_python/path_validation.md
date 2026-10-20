---
title: "Python Programming - Path Validatoin"
tags: [programming, python]
draft: false
date = "2026-10-20"
---

# Path Validatoin

傳統原則: LBYL, Look Before You Leap 三思而後行

1. os.path.exists() 檢查檔案是否存在
2. os.remove() 若存在才移除，避免 FileNotFoundError

可能問題:  
+ Race Condition: 其他程序可能在檢查結束到刪除之間的空檔改變檔案狀態。
+ Not elegant: 每次都必須重複判斷，冗長。

改使用 pathlib 模組(Python 3.4+)

```python
from pathlib import Path

Path(foo.txt).unlink(missing_ok=True)
```

或捕捉例外，處理異常: EAFP, Easier to Ask for Forgiveness than Permission 原則

```python
import os

try:
    os.remove("foo.txt")
except FileNotFoundError:
    pass # error handling
```