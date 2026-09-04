---
title: "Bug: OverlayFS Mount Point Collision"
tags: [bug, yocto, overlay]
draft: false
---

# Bug: OverlayFS Mount Point Collision

----
## Prerequisites

+ /tmp 軟連結指向 /var/volatile/tmp
+ /var/volatile 透過 fstab 掛載 tmpfs
+ systemd-tmpfiles-setup.service 讀取 tmp.conf 動態建立 /var/volatile/tmp
+ 若將 OverlayFS 掛載在 /tmp，且 service 缺少依賴順序

----
## Chronological Order of Race Condition

1. System boot
2. systemd starts Service A and Service B in parallel.
3. Service A attempts to mount OverlayFS
    + Attempt to resolve the mount path (/tmp → /var/volatile/tmp).
        + Situation 1: No /var/volatile in fstab. Mount failed.
        + Situation 2: /var/volatile is already mounted, but `systemd-tmpfiles` has not yet been executed. Mount failed.
4. Service B(systemd-tmpfiles-setup.service/fstab mount)
    + Successfully created /var/volatile/tmp.(OverlayFS failed and exited)

錯誤解析:  
1. systemd 平行排程服務: boot 階段進入 local-fs.target，systemd 同時排程處理以下服務:
    + 掛載 fstab
    + systemd-tmpfiles-setup.service
    + 掛載 OverlayFS (mnt-overlay.mount)
2. Race Trigger: OverlayFS 服務未設定以下依賴，systemd 隨機同時執行前述任務。  
    ```bash
    After=systemd-tmpfiles-setup.service
    RequiresMountsFor=/var/volatile
    ```
3. Failure Point: OverlayFS 過早掛載，系統嘗試尋找 /tmp (即 /var/volatile/tmp) ，造成以下錯誤情境:
    1. BSP 的 fstab 未設定 /var/volatile ，底層不存在 tmpfs ，OverlayFS 找不到路徑。
    2. 已掛載 /var/volatile ，但 systemd-tmpfiles 尚未建立 /var/volatile/tmp。
4. OverlayFS 找不到 Lower/Upper/Work 目錄而回傳 Error 退出
    + 即便之後 systemd-tmpfiles 建立好目錄，OverlayFS 也已經掛載失敗無法運作。

----
## Resolution

1. 解耦路徑依賴
    + OverlayFS 使用獨立掛載點 /mnt/overlay ，避免使用 /tmp 和 systemd-tmpfiles 競爭順序。
2. 滿足底層掛載要求
    + fstab 補上 /var/volatile
3. 由 systemd 接管掛載服務
    + systemd 依據正確的 target 順序 (Dependency Graph) 啟動 Mount Unit。