# Windows Packages

----
## chocolatey

安裝步驟

```powershell
# 確認執行權限, 若為 Ristricted 則執行
> Get-ExecutionPolicy
> Set-ExecutionPolicy AllSigned

# install chocolatey
> Set-ExecutionPolicy Bypass -Scope Process -Force; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))

# confirm installation
> choco
Chocolatey v2.2.2
```

操作範例

+ example packages
    + 7z.install aida64-extreme foobar2000 mingw gcc notepadplusplus.install python3 vim-x64.install sass
+ default install path: `C:\ProgramData\chocolatey\lib`

```powershell
> choco search test
> choco install test.install test1.install test2.install
> choco list
> choco upgrade test.install
> choco upgrade all
> choco upgrade all --except="'test'"
> choco uninstall test.install
```

----
## PowerShell

+ [Windows PowerShell and PowerShell are different](https://learn.microsoft.com/en-us/powershell/scripting/whats-new/differences-from-windows-powershell?view=powershell-7.3)

```powershell
> winget search Microsoft.PowerShell
#> winget install --id Microsoft.Powershell.Preview --source winget
> winget install --id Microsoft.Powershell --source winget
```