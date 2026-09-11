---
title: "Windows Package Chocolatey"
tags: [windows, package]
draft: false
---

# chocolatey

安裝步驟

```powershell
# 若執行權限為 Ristricted ，執行 Set-ExecutionPolicy AllSigned
> Get-ExecutionPolicy
Unrestricted

> Set-ExecutionPolicy AllSigned

# Install chocolatey
> Set-ExecutionPolicy Bypass -Scope Process -Force; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

# confirm installation
> choco
Chocolatey v2.7.4
```

操作範例(預設安裝路徑 `C:\ProgramData\chocolatey\lib`)  

```powershell
> choco search foobar2000
Chocolatey v2.7.4
...
foobar2000 2.25.10 [Approved]

> choco install foobar2000

> choco list | Select-String "foobar"
foobar2000 2.25.10

> choco upgrade foobar2000
...
foobar2000 v2.25.10 is the latest version available based on your source(s).

> choco upgrade all
> choco upgrade all --except="'foobar2000'"
> choco uninstall foobar2000
```