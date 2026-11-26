---
title: "Transparent Hugepage"
tags: [linux, kernel]
draft: false
date = "2026-11-26"
---

# Transparent Hugepage

核心記憶體管理。

`PMD Huge Page` 是 PMD (Page Middle Directory) Entry 能映射的記憶體空間大小。標準 4KB 分頁架構下通常為  
2MB。Linux kernel 使用多層分頁表 PGD → P4D → PUD → PMD → PTE 將虛擬地址轉換為物理地址，PMD 一般指向 PTE  
(Page Table Entry)，故單一 PMD entry 可定址 `PMD_SIZE` = 512 × 4KB = 2MB。

使用 HugePages 或 THP (Transparent Huge Pages) 時 PMD entry 會設置 Page Size bit，不指向 PTE，改指向一  
塊 2MB 連續實體記憶體。記憶體需求較高時能減少查詢分頁表 (page fault)，提高 CPU 的 TLB (Translation  
Lookaside Buffer) 命中率(降低 TLB miss)。

----
## THP and mTHP

`mTHP`, multi Transparent Huge Page

THP 僅提供 2MB/1GB 頁面，若僅使用部分 (如 2MB 頁面只使用 4KB) 則嚴重浪費；系統長期執行也容易碎片化，找不到  
連續 2MB 實體空間，導致分配失敗。Kernel 5.16 引入 `struct folio` 和 `struct pagesize` 共存，管理任意長度  
的連續記憶體。folio 允許 4KB 倍數大小如 64KB, 2MB 的彈性頁面，催生 `mTHP`, multi Transparent Huge Page。

儘管如此，碎片化問題依然存在，但優於 Classic THP。記憶體子系統也因為要因應不同場景分配、拆分、回收、合併頁面，  
使管理邏輯更加複雜，增加回收和交換頁面的壓力: 耗時、回收速率降低。

----
## Kernel Configuration

```bash
CONFIG_TRANSPARENT_HUGEPAGE=y

# The application must explicitly call madvise(..., MADV_HUGEPAGE) to request huge page allocation.
CONFIG_TRANSPARENT_HUGEPAGE_MADVISE=y

# Supports PMD-level transparent huge pages (e.g., 2 MB).
CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE=y

# Supports larger PUD-level transparent huge pages (e.g., 1 GB).
CONFIG_HAVE_ARCH_TRANSPARENT_HUGEPAGE_PUD=y

# Use the hardware's contiguous PTE bits to combine 16 contiguous 4 KB pages (64 KB mTHP)
# into a single TLB entry.
CONFIG_ARM64_CONTPTE=y

# (Optional) Supports read-only file-backed THP other than anonymous THP:
# * Read-only mmap file
# * Read-only executable mapping
# * certain file-backed memory
CONFIG_READ_ONLY_THP_FOR_FS=y

# (Optional) Supports THP page migration
CONFIG_ARCH_ENABLE_THP_MIGRATION=y

# Provides memory compaction to help obtain physically contiguous memory.
CONFIG_COMPACTION=y
```

執行期檢查設定

```bash
$ zcat /proc/config.gz | grep -E \
    'TRANSPARENT_HUGEPAGE|READ_ONLY_THP_FOR_FS|ARM64_CONTPTE|ARCH_ENABLE_THP_MIGRATION|COMPACTION'
```

----
## THP sysfs interface

+ /sys/kernel/mm/transparent_hugepage/enabled
    + 設定 THP 使用範圍
        + always: 整個系統
        + madvise: 指定地址範圍
        + never: 不使用
+ /sys/kernel/mm/transparent_hugepage/defrag
    + 設定產生頁面的方式
        + always: 同步命令；申請時 stall，直到 Kernel 準備好 hugepage。
        + defer: 非同步命令
        + defer+madvise: madvise 同步處理，其餘情況 defer。
+ /sys/kernel/mm/transparent_hugepage/khugepaged/
    + 其他參數

```bash
# Check THP sysfs interface
$ ls -l /sys/kernel/mm/transparent_hugepage/
-rw-r--r--    1 root     root          4096 Nov 16 13:34 defrag
-rw-r--r--    1 root     root          4096 Nov 16 13:34 enabled
-r--r--r--    1 root     root          4096 Nov 16 13:34 hpage_pmd_size
drwxr-xr-x    3 root     root             0 Nov 16 13:34 hugepages-1024kB
...

# Check whether THP interface 'transparent_hugepage' exists
$ ls -d /sys/kernel/mm/transparent_hugepage/hugepages-* 2>/dev/null
/sys/kernel/mm/transparent_hugepage/hugepages-1024kB
...

# Check whether classic THP 'enabled' and 'defrag' exist
$ THP=/sys/kernel/mm/transparent_hugepage && cat $THP/enabled && cat $THP/defrag
always [madvise] never
always defer defer+madvise [madvise] never

# Check whether mTHP per-size control directories exposed
$ for d in $THP/hugepages-*; do \
    [ -d "$d" ] || continue \
    echo "[$d]" \
    cat "$d/enabled" \
done
[/sys/kernel/mm/transparent_hugepage/hugepages-1024kB]
always inherit madvise [never]
[/sys/kernel/mm/transparent_hugepage/hugepages-128kB]
always inherit madvise [never]
...
```

----
## Validation

確認 THP/mTHP 確實正常運作。評估能否不過度消耗記憶體、不額外增加 CPU 負擔或延遲的情況下提升記憶體效能。

```bash
# Record the system states in order to determine whether THP allocation 
# fallbacks during testing are caused by memory fragmentation or insufficient
# available memory.
$ cat /proc/vmstat
$ cat /proc/meminfo
$ free -h
$ cat /proc/buddyinfo
$ cat /proc/pagetypeinfo
```

實驗是否如預期配置 hugepage

```bash
$ THP=/sys/kernel/mm/transparent_hugepage
# Check PMD page size(2MB)
$ cat $THP/hpage_pmd_size
2097152

# Baseline
$ echo never > "$THP/enabled"
$ echo never > "$THP/hugepages-2048kB/enabled"
$ echo never > "$THP/hugepages-64kB/enabled"

# classic THP
$ echo always > "$THP/enabled"
$ echo always > "$THP/hugepages-2048kB/enabled"
$ echo never > "$THP/hugepages-64kB/enabled"

# mTHP
$ echo always > "$THP/enabled"
$ echo never > "$THP/hugepages-2048kB/enabled"
$ echo always > "$THP/hugepages-64kB/enabled"
```

使用 stress 工具簡易測試: [thp_stress_test.sh](./inc/thp/thp_stress_test.sh)

```bash
# THP workload:
#   Start a memory worker that allocates 64 MB of memory, accesses it every 4 KB, 
#   and continuously rewrites the memory without freeing and reallocating it. 
#   Keep it running for 60 seconds.
# Purpose: Verify THP/mTHP runtime exposure and allocation activity.
#   memory allocation, first-touch behavior, and sustained read/write activity.
$ ./thp_stress_test.sh
```

預期結果:  
+ Baseline
    + With THP and all mTHP sizes disabled, no THP allocation activity was observed during the  
      stress test.
+ Classic THP
    + 2048kB `thp_fault_alloc` increased.
    + `anon_fault_alloc` increased.
+ mTHP
    + 64kB `anon_fault_alloc` increased: 64KB mTHP fault allocation has occurred.