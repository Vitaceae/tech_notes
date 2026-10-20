---
title: "Git LFS"
tags: [git, lfs]
draft: false
---

# Git LFS, Large File Storage

----
## Skip downloading from LFS 

```bash
$ GIT_LFS_SKIP_SMUDGE=1 git clone <repo_url>

# on Windows
$ set GIT_LFS_SKIP_SMUDGE=1
$ git clone <repo_url>
```

----
## Configure git-lfs smudge

```bash
# Configuring git-lfs smudge
$ git config --global filter.lfs.smudge "git-lfs smudge --skip -- %f"
$ git config --global filter.lfs.process "git-lfs filter-process --skip"
$ git clone <repo url>

# Undo configuration
$ git config --global filter.lfs.smudge "git-lfs smudge -- %f"
$ git config --global filter.lfs.process "git-lfs filter-process"
```

----
## File Locking

最低支援版本: Git LFS v2.0.0，建議使用 v2.2 以上。

File Locking API:  

```bash
# for remote repo
$ git config lfs.https://<server>/<repo path>.git/info/lfs.locksverify true
# for current repo
$ git config lfs.locksverify true
$ git config --local --get lfs.locksverify

$ git lfs install
$ git lfs locks --local
$ git lfs locks --verify

# Track all .exe files
$ git lfs track *.exe --lockable

# Acquire a lock
$ git lfs lock

# Release a lock
$ git lfs unlock
```

----
## Problems

>   x509: certificate signed by unknown authority

```bash
# 1. Configure Git/Git LFS to trust the certificate separately.
$ git config http.sslCAInfo <foo_ca.crt>

# 2. Disable SSL verification(with security risks).
$ git config http.sslVerify false
# Restore the secure settings.
# git config --unset http.sslVerify

# 3. Only for this LFS download command.
$ GIT_SSL_NO_VERIFY=true git lfs pull
```

----
## References

[Official: Git Large File Storage](https://git-lfs.com/)  