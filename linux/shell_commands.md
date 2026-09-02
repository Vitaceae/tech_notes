---
title: "Linux Shell Commands"
tags: [linux, shell, bash]
draft: false
---

# Linux Shell Commands

----
## find
  
```bash
# 列出匹配檔案及其屬性
$ find / -type f -size +100M -exec ls -lh {} \;
# 僅列出檔案大小及檔名
$ find / -type f -size +100M -exec ls -lh {} \; | awk '{print $5, $9}'
# 加強處理效率(傳入多參數)
$ find / -type f -size +100M -exec du -h {} +
# 隱藏 Permission denied 訊息並排序
$ find / -type f -size +100M 2>/dev/null -exec du -h {} + | sort -hr
# 避免檢查其他掛載點
$ find / -xdev -type f -size +100M 2>/dev/null -exec du -h {} + | sort -hr | head
```

----
## Others

讀取分割區特定偏移量: 128KB 位置起算的 16bytes hex 資料

```bash
$ dd if=/dev/foo bs=1 skip=$((128*1024)) count=16 2>/dev/null | xxd
# without ASCII codes
$ dd if=/dev/foo bs=1 skip=$((128*1024)) count=16 2>/dev/null | xxd -p
# faster
$ dd if=/dev/foo bs=1024 skip=128 count=1 2>/dev/null | head -c 16 | xxd
# replace xxd with hexdump
$ dd if=/dev/foo bs=1024 skip=128 count=1 2>/dev/null | hexdump -C | head -n1
# without dd
$ xxd -s 0x20000 -l 16 /dev/foo
```
