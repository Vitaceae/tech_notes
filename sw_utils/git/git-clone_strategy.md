---
title: "Git Clone Stragety"
tags: [git]
draft: false
date = "2026-10-20"
---

# Git Clone Stragety

每個 commit 對應一次完整提交紀錄；tree 記錄目錄和其下檔案；blob 紀錄檔案內容或差異。

----
## Clone

+ 下載完整 commit history
+ 適用長期離線工作、須查詢歷史提交紀錄、傳輸成本小、下載速度不影響工作等任務。

----
## Shallow Clone 淺複製

+ 控制提交歷史深度，僅下載近期 commit history。
+ 只需要測試近期快照，不須參閱完整歷史提交。
要追查或比較分支，但只改少數目錄：先評估 blobless Partial＋Sparse。
任務橫跨全專案、常離線或下載成本很小：完整 clone 是容易理解的基準。

```bash
$ git clone --depth=1 <repo_url>

# 判斷是否 shallow clone
$ git rev-parse --is-shallow-repository

# 統計目前分支包含多少次 commit
$ git rev-list --count HEAD
```

----
## Partial Clone 部分複製

暫緩下載指定 git 物件類型；需要時(如 checkout 或檢索紀錄)才下載。核心概念為「約定遠端(Promisor Remote)」，即
承諾可在未來需要時供應所需檔案的遠端儲存庫。工作原理為 On-demand Fetching。

+ `Blobless Clone`: 只下載 commit 和 tree，延後下載 blob。
+ `Treeless Clone`: 只下載 commit，延後下載 tree 和 blob。
+ 一般搭配 `--no-checkout` 使用，否則 main branch 檢出檔案時即開始下載。

```bash
# Blobless Clone
$ git clone --filter=blob:none <repo_url>

# Treeless Clone
$ git clone --filter=tree:0 <repo_url>
```

----
## Sparse checkout

工作目錄(working tree)中只檢出指定的資料夾，節省磁碟空間。

+ Cone Mode:  
    + --cone
    + 目標單位為目錄；包含指定目錄下的所有子目錄與檔案。
+ Non-Cone Mode: 
    + --no-cone
    + Pattern-based/Glob 規則
    + 不推薦使用；僅在需要複雜、非目錄結構的檔案過濾規則時才會用。

```bash
$ git sparse-checkout set --cone <dir>
```

----
## Practice

[git_clone_strategy.py](./inc/git_clone_strategy.py) 產生本機 repo，用於測試不同策略下資源消耗量。

```bash
$ python git_clone_strategy.py
Initialized empty Git repository in /home/mike/work/git_clone_strategy/src/.git/
[main (root-commit) d572fae] test-0
 4 files changed, 6 insertions(+)
 create mode 100644 README.md
 create mode 100644 apps/web/check.py
 create mode 100644 assets/foo.bin
 create mode 100644 packages/shared/price.py
[main 2089cea] test-1
 2 files changed, 1 insertion(+), 1 deletion(-)
[main 4f75b71] test-2
 2 files changed, 1 insertion(+), 1 deletion(-)
[main 81accc9] test-3
 2 files changed, 1 insertion(+), 1 deletion(-)
[main 7c1cf25] test-4
 2 files changed, 1 insertion(+), 1 deletion(-)
Switched to branch 'feature'
[feature 5045527] feature
 1 file changed, 1 insertion(+)
 create mode 100644 feature.txt
Switched to branch 'main'
Cloning into bare repository '/home/mike/work/git_clone_strategy/origin.git'...
done.
remote=file:///home/mike/work/git_clone_strategy/origin.git
7c1cf2525e17981cce2f13e91f79ff2d1328d87b
```

設定目錄 git_clone_strategy 為 remote source，測試 full/shallow/partial 複製時的資源開銷。變數  
GIT_TRACE_PACKFILE 記錄接收到的資料流。僅計算磁區的 .pack 檔案可能漏算 fetch 時解包的散落物件。以上測試流量  
忽略實際使用時網路、協定、驗證等開銷。

```bash
# Use the path specified above
$ remote_url=file:///home/mike/work/git_clone_strategy/origin.git

# Full clone
$ export GIT_TRACE_PACKFILE="$PWD/full.packstream"
$ git clone "$remote_url" full_clone
$ wc -c full.packstream

# Shallow Clone
$ export GIT_TRACE_PACKFILE="$PWD/shallow.packstream"
$ git clone --depth=1 "$remote_url" shallow_clone
$ wc -c shallow.packstream

# Partial Clone
$ export GIT_TRACE_PACKFILE="$PWD/partial.packstream"
$ git clone --filter=blob:none --no-checkout "$remote_url" partial_clone
$ wc -c partial.packstream
# Sparse checkout
$ export GIT_TRACE_PACKFILE="$PWD/sparse.packstream"
$ git -C partial_clone sparse-checkout set --cone apps/web
$ git -C partial_clone checkout 7c1cf2525e17981cce2f13e91f79ff2d1328d87b
$ wc -c sparse.packstream

$ unset GIT_TRACE_PACKFILE
```

檢查實際佔用的磁碟空間

```bash
# target: full_clone, shallow_clone, partial_clone
$ target=full_clone
$ du -sk $target/.git
$ du -sk $target
```

執行 check.py 檢查運作是否正常。

```bash
# target: full_clone, shallow_clone, partial_clone
$ target=full_clone
$ pushd $target; python -B -c "import runpy;runpy.run_path('apps/web/check.py')"; popd
```

full clone 和 shallow clone 執行結果為 AssertionError，partial clone 回報 ModuleNotFoundError。表示部分  
下載的檔案有所缺失。使用 sparse-checkout 修正 partial clone 造成的錯誤。

```bash
$ git -C partial_clone sparse-checkout list
apps/web

$ git -C partial_clone ls-tree -r --name-only HEAD
README.md
apps/web/check.py
assets/foo.bin
packages/shared/price.py

$ git -C partial_clone sparse-checkout add packages/shared
```

再次執行 check.py，回報 AssertionError，已補齊缺漏的檔案。  

下一步，嘗試回溯歷史修正 AssertionError，檢查是否有造成程式出錯的提交。

```bash
$ git -C shallow_clone fetch --depth=1 origin feature:refs/remotes/origin/feature

# No common ancestor
$ git -C shallow_clone merge-base HEAD origin/feature

$ git -C shallow_clone blame HEAD -- packages/shared/price.py
^7c1cf25 (foo 2026-10-05 00:00:00 +0000 1) def price(x):
^7c1cf25 (foo 2026-10-05 00:00:00 +0000 2)     return x + 1
```

feature 分支和 main 分支沒有共同祖先；當前分支修改紀錄僅包含根提交 ^7c1cf25，需要檢查更早的提交紀錄。範例一  
次性補回所有提交。

```bash
$ git -C shallow_clone fetch --unshallow origin

$ git -C shallow_clone rev-parse --is-shallow-repository
false

$ git -C shallow_clone merge-base HEAD origin/feature
2089ceacc153f90b24c6c456182be2f9bd978f18

$ git -C shallow_clone blame HEAD -- packages/shared/price.py
```