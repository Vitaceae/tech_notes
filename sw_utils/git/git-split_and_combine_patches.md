---
title: "Git - Split and Combine Patches"
tags: [git]
draft: false
---

# Git - Split and Combine Patches

非 Git 專用；將變更複數檔案的 patch 拆分成僅修改單一檔案的許多 patch，重新組成新 patch。  
缺點: 拆解過程中會遺失 author 及 timestamp 等 header。可使用 git apply 套用變更，但無法使用 git am 。  

```bash
$ splitdiff --help
usage: splitdiff [OPTIONS] FILE
OPTIONS are:
  -a              split out every single file-level patch
  -p N            pathname components to ignore
  -d              use output filenames like a_b.c.patch for a/b.c
  -D dir          create patches in subdirectory dir (implies -d)
  -E              don't use .patch filename extension

$ git show --stat ff028c0
commit ff028c05d283ef71baaf0c53c93d0752ce5acf7e (HEAD -> master)
Author: Mike.Chang ...
Date:   Wed Sep 16 21:38:18 2026 +0800

    splitdiff test

 foo-1.txt | 1 +
 foo-2.txt | 1 +
 foo-3.txt | 1 +
 3 files changed, 3 insertions(+)

$ git format-patch -1 ff028c0
0001-splitdiff-test.patch

$ splitdiff -a 0001-splitdiff-test.patch
Wrote >0001-splitdiff-test.patch.part001.patch
Wrote >0001-splitdiff-test.patch.part002.patch
Wrote >0001-splitdiff-test.patch.part003.patch
```

使用 cat 重新組合 patch

```bash
$ cat 0001-splitdiff-test.patch.part001.patch  0001-splitdiff-test.patch.part002.patch > 1.patch
```