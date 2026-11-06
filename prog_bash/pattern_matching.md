---
title: "Bash Programming - Example Usage"
tags: [programming, bash]
draft: false
date = "2026-11-06"
---

## Pattern Matching

字串匹配:  
|Syntax                 |Example: var="images/photo.jpg"    |Description    |
|:----------------------|:----------------------------------|:--------------|
|`${var#pattern}`       |`${var#*/}`    → "photo.jpg"       |從開頭刪除最短匹配|
|`${var##pattern}`      |`${var##*o}`   → ".jpg"            |從開頭刪除最長匹配|
|`${var%pattern}`       |`${var%.*}`    → "images/photo"    |從結尾刪除最短匹配|
|`${var%%pattern}`      |`${var%%o*}`   → "images/ph"       |從結尾刪除最長匹配|
|`${var/pattern/str}`   |`${var/o/X}`   → "images/phXto.jpg"|替換第一個匹配到的字串|
|`${var//pattern/str}`  |`${var//o/X}`  → "images/phXtX.jpg"|替換所有匹配到的字串|


Bash 模式匹配機制:  
+ Globbing 通配符
+ Extended Globbing 擴充通配符
+ Regex 正規表示式

----
## Globbing

+ `*` 匹配任意字串，包含空字串。
+ `?` 匹配任意字元。
+ `[...]`: 匹配括弧內任意字元，與 Regex 相同。
    + `[!...]` 或 `[^...]` 表示排除指定字元。
    + `[a-zA-Z]` 表示匹配任意英文字元，無論大小寫。
    + `[:alnum:]` 表示 Alphanumeric，接受任意字母與數字。

----
## Extended Globbing

```bash
# enable shell option 'extglob'
shopt -s extglob

# disable shell option 'extglob'
shopt -u extglob
```

+ `?(pattern-list)` 滿足開頭外，匹配 list 成員零或一次。
+ `*(pattern-list)` 滿足開頭外，匹配 list 成員零或多次。
+ `+(pattern-list)` 滿足開頭外，匹配 list 成員一次以上。
+ `@(pattern-list)` 滿足開頭外，匹配 list 成員正好一次。
+ `!(pattern-list)` 滿足開頭外，不匹配 list 成員。

Example 1:

```bash
$ shopt -s extglob
# foo.txt, foo.bak, foo.bak.bak, bar.doc
$ ls file*(.bak)
foo.txt foo.bak foo.bak.bak

# foo.txt: 匹配 foo 後出現 0 次 .bak
# foo.bak: 匹配 foo 後出現 1 次 .bak
# foo.bak.bak: 匹配 foo 後出現 2 次 .bak
# bar.doc: 未匹配 foo
```

Example 2:

```bash
$ shopt -s extglob
# 移除特定檔案之外的所有檔案
$ rm -- !($file_kept)
```

----
## Regex

在 `[[...]]` 結構中 `=~` 運算子可使用 POSIX 擴充正規表示式。

Example:

```bash
# 匹配 四位數字-兩位數字-兩位數字
regex='^([0-9]{4})-([0-9]{2})-([0-9]{2})$'
date_str="2026-11-06"

if [[ "$date_str" =~ $regex ]]; then
    echo "full match: ${BASH_REMATCH[0]}" # 2026-11-06
    echo "year: ${BASH_REMATCH[1]}"       # 2026
    echo "month: ${BASH_REMATCH[2]}"      # 11
    echo "day: ${BASH_REMATCH[3]}"        # 06
fi
```