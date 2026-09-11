---
title: "Windows Package Powershell"
tags: [windows, package]
draft: false
---

# Windows Powershell

手動安裝

```powershell
> winget search Microsoft.PowerShell
#> winget install --id Microsoft.Powershell.Preview --source winget
> winget install --id Microsoft.Powershell --source winget
```

常用變數

+ %USERPROFILE%
+ %APPDATA%
+ %LOCALAPPDATA%

----
## References

[Windows PowerShell and PowerShell are different](https://learn.microsoft.com/en-us/powershell/scripting/whats-new/differences-from-windows-powershell?view=powershell-7.3)  