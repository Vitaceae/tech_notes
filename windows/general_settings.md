# Windows General Settings

Windows 設定筆記

----
## Enable telnet feature

```powershell
dism /online /Enable-Feature /FeatureName:TelnetClient
```

----
## Set up static IP

1. 工作列 → 開始 → 設定 → 網路和網際網路 → 乙太網路 → IP 指派 → 編輯
2. 編輯IP設定(e.g. 對象IP 192.168.18.32)
    + 設定 `手動`
    + 設定 `IPv4`: 開啟
    + 設定 `IP 位址`: 192.168.18.2
    + 設定 `子網路遮罩`: 255.255.255.0
    + 設定 `閘道`: 192.168.18.254
    + 設定 `慣用的 DNS`: 192.168.18.1

----
## Windows 11

### mouse menu

```cmd
:: Windows 10 style
> reg add "HKCU\Software\Classes\CLSID\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}\InprocServer32" /f /ve
> taskkill /f /im explorer.exe & start explorer.exe

:: Windows 11 style
> reg delete "HKCU\Software\Classes\CLSID\{86ca1aa0-34aa-4e8b-a509-50c905bae2a2}" /f 
> taskkill /f /im explorer.exe & start explorer.exe
```

----
## Windows Terminal

設定檔路徑: `%LOCALAPPDATA%\Packages\Microsoft.WindowsTerminal_8wekyb3d8bbwe\LocalState\settings.json`

使用錯誤提示音取代螢幕閃爍

```bash
# none, audible, visible
$ echo "set bell-style none" >> ~/.inputrc
```

auto-complete 忽略大小寫

```bash
$ echo "set completion-ignore-case On" >> ~/.inputrc
```

----
## Snippets

變更 IE 字型: [registry](inc/win-registry-change-ie-fonts.reg)

變更系統字型: [registry](inc/win-registry-system_fonts.reg)  

原始系統字型: [initial-setting](inc/win-registry-system_fonts.bak.reg)  

定時關機  
+ [60s](inc/win-shortcut-timed-shutdown-60s)  
+ [2300](inc/win-shortcut-timed-shutdown-2300)  
+ [disable](inc/win-shortcut-timed-shutdown-disable)  

啟用 KMS/OEM 序號  

```powershell
# Admin

# 查詢啟用狀態
slmgr.vbs -xpr

slmgr /ipk VK7JG-NPHTM-C97JM-9MPGT-3V66T
slmgr /skms kms.xspace.in
slmgr /ato
```

電池使用時間報告

```powershell
> powercfg /batteryreport
```


----
## Change account name

1. Control Panel → User Accounts → Change your account name
2. run → regedit
    + Key_Local_Machine/Software/Microsoft/Windows NT/CurrentVersion/ProfileList
    + modify ProfileImagePath

change local users and groups

+ run → lusrmgr.msc
    + Users → <YourAccount> → <right-click> → Rename

group policy editor

+ run → gpedit.msc
    + Computer Configuration → Windows Settings → Security Settings → Local Policies → Security Options
    + Rename administrator account and guest account

user management

+ run → netplwiz
    + <YourAccount> → <right-click> → Properties

----
## Network

+ 控制台 → 網路和網際網路 → 網路連線: 右鍵 "連線圖示" → 共用 → 允許其他使用者

### Network Drive

```cmd
; clear password and disconnect network drives
net use * /del
;   with specified drive name
net use X: /delete
;   with specified IP
net use \\192.168.1.123 /delete

; mount network drive
net use Z: \\[ip]\[dir] /user:[user_name] [user_password]
```

### Settings

```cmd
; %windir%\System32\drivers\etc\hosts
;   C:\Windows\System32\drivers\etc\hosts

127.0.0.1   domain-name

; IIS website binding
;   IIS Manager::目標站台::Bindings
;       Add::Host name::OK
```

----
## Problems

>   因為應用程式的並列設定不正確，所以無法啟動

事件檢視器

```
"C:\Users\i7017\workspace\build\common\bin\LicenseManager.exe" 的啟用內容產生失敗。
找不到依存組合
Microsoft.VC90.MFC,
processorArchitecture="x86",
publicKeyToken="1fc8b3b9a1e18e3b",
type="win32",
version="9.0.21022.8"。
請使用 sxstrace.exe 進行詳細的診斷。
```

need `MSVCR80.dll`; install `vcredist_x86_b8fab0bb7f62a24ddfe77b19cd9a1451abd7b847.exe`

>   WMI Provider Host 佔用 CPU

1. Event Viewer(事件檢視器) → 應用程式及服務記錄檔 → Microsoft → Windows → WMI-Activity → Operational
2. 錯誤資訊: 雙擊 → 詳細資料
3. ClientProcessId 造成 VMI 錯誤, 佔用 CPU
4. 開啟工作管理員 尋找 PID, 解除安裝或禁用該程序

>   字體模糊; DPI 錯誤

put [dpi_fix.cmd](./inc/dpi_fix.cmd) in `C:\Users\mai\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup`

>   筆電關機異常耗電

+ 裝置管理員 → 網路介面卡 → 內容 → 電源管理 → 允許從這個裝置喚醒電腦: 取消勾選
+ 裝置管理員 → 網路介面卡 → 內容 → 進階 → Wake...: 取消勾選

>   找不到 msvcr100.dll 或 msvcr100.dll

Visual Studio 2015 的 Visual C++ 可轉散發套件
+ 安裝 [Microsoft Visual C++ Redistributable Package](https://www.microsoft.com/zh-tw/download/details.aspx?id=48145)
+ 從註冊表尋找遺失檔案
    1. 執行 regedit
    2. 網址列輸入: `電腦\HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\SharedDLLs`
    3. 將 `msvcr100.dll` 複製到適當位置(如 `C:\Windows\system32\msvcr100.dll`)
    4. 重新啟動
