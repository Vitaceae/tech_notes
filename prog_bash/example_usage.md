---
title: "Bash Programming - Example Usage"
tags: [programming, bash]
draft: false
date = "2026-11-06"
---

# Bash Example Usage

各種情境使用範例

----
## Copy

```bash
# Copy with exclusion
$ rsync -av --progress $dir_src $dir_dest --exclude $dir_exlude

# Copy piped output
# -l: --files-with-matches, 不顯示匹配內容
$ grep -l -r "$keyword" --exclude=$exclude_pattern | xargs cp -t $dir_dest
```

----
## Remove

```bash
# 搜尋特定目錄並移除
$ find . -name ".svn" -type d -exec rm -r "{}" \;
# 搜尋特定空目錄並移除
$ find . -name ".svn" -type d -empty -delete

# 列出當前目錄下所有檔案，排除特定檔案後移除
$ ls | grep -v .git | xargs rm -rf
$ rm -f !(a|b)

# delete symbolic links
$ unlink
```

----
## Modify

```bash
# trim trailing whitespace
$ sed -i 's/[[:space:]]*$//' *
```

----
## Search and Filter

```bash
# 搜尋特定檔名後匹配檔案內容特定字串
# -name: case-sensitive
# -iname: case-insensitive
$ find . -iname "$filename" | xargs grep $text

# 列出當前目錄下包含子目錄的所有檔案，排除指定檔案後移除。使用 -type d 改為刪除目錄
$ find . ! -name '$file_kept' -type f -exec rm -f {} +
# 限定最大目錄深度為 1
$ find . -maxdepth 1 ! -name '$KEEP_FILE' -type f -exec rm -f {} +

# 搜尋並重命名檔案
find . -type f -name "$filename" | \
    while read -r f; do \
        mv "$f" "${f//old_name/new_name}"; \
    done

# remove empty files
$ find ./ -size 0 -exec rm {} \;
$ rm -i `find ./ -size 0`
$ find ./ -size 0 | xargs rm -f &

# match file info
$ find . -type f -exec ls -l { } \;

# remove files modified within 5 days
$ find /logs -type f -mtime +5 -exec rm { } \; 

# file size larger than
$ find . -type f -size +100M
```

----
## List

```bash
# list counts
$ ls -1 | wc -l

# list files starting with 'PREFIX'
$ ls -1 | grep ^PREFIX
```

----
## 找出系統中所有 32-bit ibnary

```bash
$ find / -type f -exec file {} + 2>/dev/null | grep 'ELF 32-bit'
# 僅列出路徑
$ find / -type f -exec file {} + 2>/dev/null | awk -F: '/ELF 32-bit/ {print $1}'

# 使用 ELF header 的 EI_CLASS 欄位判斷: 1=ELF32, 2=ELF64
#   od, Octal Dump: 將輸入內容轉換成指定進位格式
#   -An: Address base=none, 不顯示左側檔案位址偏移量
#   -tu1: 輸出格式 u1, unsigned decimal, 1-byte
#   -j4: skip-bytes=4
#   -N1: read limit=1
find / -type f -exec sh -c '
for f do
    [ "$(od -An -tu1 -j4 -N1 "$f" 2>/dev/null | tr -d " ")" = "1" ] &&
        echo "$f"
done
' sh {} +
```

----
## Others

```bash
# split and compress
$ split --bytes=128M --filter='gzip > $FILE.gz' rootfs.img.demo1 rootfs.out
$ split --bytes=256M --numeric-suffixes=1 --filter='gzip > $FILE.gz' rootfs.img.demo1 rootfs.out.
$ rename 's/\.0+/./' rootfs.out.0?

# bulk rename
$ ls * | sed -e 'p;s/foo/bar/' | xargs -n2 mv

# copy with exclusion
$ rsync -av --progress [dir_src] [dir_dest] --exclude [dir_exlude]

# remove with exclusion
$ ls |grep -v .git |xargs rm -rf
$ rm -f !(a|b)

# remove dir
$ find . -name ".svn" -type d -exec rm -r "{}" \;
$ find . -name ".svn" -type d -empty -delete

# pipe output
$ KEYWORD="test"; PATTERN="pat"; OUT_PATH=.
$ grep -l -r "$KEYWORD" --exclude=$PATTERN | xargs cp -t $OUT_PATH

# trim trailing whitespace
$ sed -i 's/[[:space:]]*$//' *

# flush
$ sync

# number and sort
$ sort -n > number.data << end!

# encoding from DOS to Unix style
$ find . -type f -print0 | xargs -0 dos2unix
```