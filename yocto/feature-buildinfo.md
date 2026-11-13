---
title: "Yocto feature - Buildinfo"
tags: [feature, yocto]
draft: false
date = "2026-11-13"
---

# Buildinfo

Yocto `image-buildinfo` Class 可在建構時將建構資訊寫入 rootfs，在運行時獲得當前鏡像資訊。

在全域 conf/local.conf 內宣告

```
INHERIT += "image-buildinfo"
IMAGE_BUILDINFO_VARS:append = " DATETIME"
buildinfo[vardepsexclude] += "DATETIME"
```

或在 image recipe 內宣告

```python
inherit image-buildinfo
IMAGE_BUILDINFO_VARS:append = " DATETIME IMAGE_FEATURES DEBUG_BUILD"
buildinfo[vardepsexclude] += "DATETIME"
```

範例輸出:  

```bash
$ cat build/tmp/work/.../my-image-name/.../rootfs/etc/buildinfo
-----------------------
Build Configuration:  |
-----------------------
DISTRO = ...
DISTRO_VERSION = ...
DATETIME = ...
DATETIME = ...
IMAGE_FEATURES = allow-root-login ...
DEBUG_BUILD =
-----------------------
Layer Revisions:      |
-----------------------
meta-base         = ... -- modified
meta-bsp          = ... -- modified
meta-foo          = ...
...
```

常用變數:
+ DATETIME: 格式為 YYYYMMDDhhmmss 如 20261113153900。
+ DATE: 格式為 YYYYMMDD 如 20261113。
+ TIME：格式為 hhmmss 如 153900。

>   vardepsexclude 的作用?

`DATETIME` 會在 BitBake 執行期變動，導致 basehash value 變更，編譯時可能跳出 task hash 不一致的錯誤。

>   /etc/buildinfo 再次編譯卻不變?

如果 rootfs 內容不變，系統沿用 Sstate Cache，時間可能停留在最近一次實際執行 do_rootfs 的時間點。

>   production image 不建議加入?

1. IMAGE_BUILDINFO_VARS 預設值只有 DISTRO DISTRO_VERSION，為了確保可重現建置 Reproducible Builds 與  
   維持 sstate cache 穩定，需排除隨時間變動的變數如 DATE、DATETIME。
2. 可能洩漏建置組態資訊。

----
## Self-defined timestamp

```python
f_add_timestamp() {
    date -u +"%Y-%m-%dT%H:%M:%SZ" > ${IMAGE_ROOTFS}/etc/build-date
}
ROOTFS_POSTPROCESS_COMMAND += "f_add_timestamp; "
```
