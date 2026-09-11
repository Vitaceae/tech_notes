---
title: "Bash Examples"
tags: [bash]
draft: false
---

# Bash Examples

紀錄腳本指令

----
## 檢索檔案

搜尋檔名包含 <pattern> 的檔案，限定目錄深度。

```bash
# {}: A placeholder for the filenames found by find.
# +: pass filenames to a single grep command other than running grep for each file
$ find . -maxdepth N -type f -exec grep "<pattern>" {} +
```

搜尋檔名包含 <pattern> 的檔案，限定目錄深度，輸出檔案內容。

```bash
$ find ./tmp -maxdepth 1 -name "*.txt" -print -exec cat "{}" \;
```
