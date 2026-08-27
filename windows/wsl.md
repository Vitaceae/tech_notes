# WSL, Windows Subsystem for Linux

WSL 設定筆記。僅針對 WSL2 以及 Windows 20H1-18917.1000 之後版本。

----
## Enable WSL(Optional)

若預設已啟用則跳過

1. 檢查並修復系統檔案
    ```powershell
    > sfc /scannow
    ```
    + 若系統資源被鎖定/檔案受損
        ```powershell
        > DISM.exe /Online /Cleanup-image /Restorehealth
        > RD /S /Q %SystemDrive%\windows.old
        ```
    + 若存取被拒: 磁碟清理 → 清理系統檔案 → 之前的 Windows 安裝 → 確定
2. 設定 → 更新和安全 → 診斷和意見反應 → 完整
3. 設定 → 更新和安全 → Windows 測試人員計畫 → 開始 → 使用 Microsoft 帳戶註冊 Windows 測試人員計畫 → 快速
4. Windows Update → 檢查更新
    + 若更新停止太久
        ```powershell
        > net stop wuauserv
        > net stop bits
        ```
        刪除 C:\Windows\SoftwareDistribution 下所有檔案
        ```powershell
        > net start wuauserv
        > net start bits
        ```
5. Enable WSL2
    ```powershell
    > dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart

    # enable Hyper-V
    #> DISM /Online /Enable-Feature /All /FeatureName:Microsoft-Hyper-V
    > Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Hyper-V -All
    ```

----
## Install Distro

### from Microsoft App Market

```powershell
> wsl --install --distribution ubuntu
```

Microsoft 提供歷史 [Linux 發行版](https://learn.microsoft.com/zh-tw/windows/wsl/install-manual)

### from Docker Image

Download rootfs image from [Ubuntu Release Images](https://cloud-images.ubuntu.com/releases/) 或 [Ubuntu WSL Images](https://cloud-images.ubuntu.com/wsl/).

使用檔名含 wsl 的鏡像如 focal-server-cloudimg-amd64-wsl.rootfs.tar.gz

```powershell
# e.g. wsl --import ubuntu-20.04 D:\work\ubuntu-20.04 D:\work\focal-server-cloudimg-amd64-wsl.rootfs.tar.gz --version2
> wsl --import <distor name> <install path> <image path> --version 2
```

----
## Run Distro

以系統管理員身分執行 powershell

```powershell
# find installed distro
> wsl -l

# set WSL default version
> wsl --set-default-version 2
> wsl --set-version <distro name> 2

# check WSL version
> wsl -l -v
> wsl --list --verbose

# start distro with a specific user
> wsl -d <distro name> -u <user name>

# exit distro
> wsl --terminate <distro name>

# shutdown all WSL instances
> wsl --shutdown
```

----
## Upgrade Linux kernel

### official image

+ [Release Notes for Windows Subsystem for Linux kernel](https://docs.microsoft.com/en-us/windows/wsl/kernel-release-notes)
+ [Github: microsoft WSL2 Linux Kernel](https://github.com/microsoft/WSL2-Linux-Kernel)

```powershell
> wsl --update
```

### custom image

+ [ref: Build instructions](https://github.com/microsoft/WSL2-Linux-Kernel/blob/7015d6023d60b29c3be4c6a398bed923b48b4341/README-Microsoft.WSL2)
+ [ref: Optimized kernel config for better general kernel support and saner defaults](https://github.com/microsoft/WSL2-Linux-Kernel/pull/176)
+ [Advanced settings configuration in WSL](https://docs.microsoft.com/en-us/windows/wsl/wsl-config#wsl-2-settings)


```bash
$ sudo apt install -y build-essential flex bison libssl-dev libelf-dev

# get kernel stable branch
$ git clone --depth 1 --branch linux-rolling-stable https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git
$ cd linux
$ git checkout linux-rolling-stable

# optimized kernel config for WSL2
$ wget https://github.com/microsoft/WSL2-Linux-Kernel/blob/7015d6023d60b29c3be4c6a398bed923b48b4341/Microsoft/config-wsl -O .config
# set LOCALVERSION
$ sed -i 's/microsoft-standard-WSL2/generic/' ./.config
# (Optional) update config file to include the newest Kernel options before compilation

$ make prepare
Preload BPF file system with kernel specific program and map iterators (BPF_PRELOAD) [N/y/?] (NEW) y

$ make -j $(nproc)

# install optional modules
$ sudo make modules_install

$ mkdir /mnt/c/wslkernel
$ cp arch/x86/boot/bzImage /mnt/c/wslkernel/custom_kernel
```

Windows 的 `.wslconfig` 需指定使用自定義 kernel

```bash
[wsl2]
kernel = c:\\wslkernel\\custom_kernel
```

----
## Export and Import Distro

```powershell
> wsl --shutdown

> wsl --export <distro name> <export path>\<export name>.vhdx --vhd
> wsl --unregister <distro>

> wsl --import <distro name> <install path> <import name>.vhdx --vhd
```

透過註冊檔安裝: [registry template](inc/wsl/vhdx.reg)  
+ SID
    ```powershell
    > whoami /user
    ```
+ UUID: [online UUID Generator](https://www.uuidgenerator.net/)
+ DISTRO_NAME
+ VHDX_PATH

----
## Configure WSL

### 分配虛擬機資源

Windows 目錄建立 `%UserProfile%\.wslconfig`，個人設定: [.wslconfig](inc/wsl/.wslconfig)

### 設定虛擬機功能

虛擬機目錄建立 `/etc/wsl.conf`，個人設定: [wsl.conf](inc/wsl/wsl.conf)

Note: automount 設定可能導致 Windows 執行檔出錯(timeout)

### 關閉提示音

TAB 鍵執行 auto-complete 時產生錯誤提示音

```bash
$ sudo vi /etc/inputrc

set bell-style none

$ sudo vi /etc/vim/vimrc

set vb t_vb=
```

### (Optional) Remove Windows-Style PATH

wsl.conf 設定 `appendWindowsPath` 為 false 後應該不再需要?

```bash
$ vi ~/.bashrc

export PATH=$(echo $PATH | tr ':' '\n' | grep -v /mnt/ | tr '\n' ':')
export PATH=/mnt/c/Windows:/mnt/c/Windows/System32:$PATH
export PATH=/mnt/c/Windows:$PATH
```

----
## Trim WSL File System Size

On WSL

```bash
$ sudo fstrim /
```

On Windows

```powershell
> diskpart
# C:\Users\*\AppData\Local\wsl\{*}\ext4.vhdx
> select vdisk file="<vhdx_path>\ext4.vhdx"
> attach vdisk readonly
> compact vdisk
> detach vdisk
> exit

# Windows Pro(Hyper-V)
> wsl --shutdown <distro>
> Optimize-VHD -Path "<vhdx_path>\ext4.vhdx" -Mode full
```

----
## Expand WSL File System Size

+ default limit: 256GB

On Windows

```powershell
> wsl --shutdown <distro>
> diskpart
> select vdisk file=<vhdx_path>
> expand vdisk maximum=<size_mb>
```

On WSL

```bash
$ sudo apt install -y cloud-guest-utils xfsprogs
#sudo mount -t devtmpfs none /dev
#mount | grep ext4
$ sudo resize2fs /dev/sdXX

# e.g. size_mb=512*1024 (512GB)
$ df -h

Filesystem      Size  Used Avail Use% Mounted on
/dev/sda        503G  105G  376G  22% /
```

----
## Audio

(未驗證) 已原生支援，不必繁瑣設定 pulseaudio

----
## GUI

### Native method

+ [在 Windows 子系統 Linux 版上執行 Linux GUI 應用程式](https://learn.microsoft.com/zh-tw/windows/wsl/tutorials/gui-apps)

```bash
$ sudo apt update

# 安裝 Gnome 桌面環境預設文字編輯器; 取代 Ubuntu 22+ 以前的 gedit
$ sudo apt install gnome-text-editor -y
# 舊版 Ubuntu 使用 gedit
$ sudo apt install gedit -y

# (optional) 在編輯器中啟動 bashrc 檔案; 
$ gnome-text-editor ~/.bashrc

# 安裝 GIMP 點陣圖形編輯器
$ sudo apt install gimp -y

# 安裝 Nautilus 桌面檔案管理器
$ sudo apt install nautilus -y

# 安裝 VLC 多媒體播放器
$ sudo apt install vlc -y

# 安裝 X11 視窗系統
$ sudo apt install x11-apps -y

# 安裝 Google Chrome
$ mkdir -p temp; cd /tmp
$ wget https://dl.google.com/linux/direct/google-chrome-stable_current_amd64.deb
$ sudo apt install --fix-missing ./google-chrome-stable_current_amd64.deb
```

### Use Xfce

1. install Xfce GUI on WSL
    + a lightweight desktop environment
    + [XLaunch: XLaunch Wizard](http://www.straightrunning.com/xmingnotes/IDH_DISPLAY.htm)
    + [XLaunch: Additional parameters](http://www.straightrunning.com/xmingnotes/IDH_CLIPBOARD.htm)
    + settings(1280x720, offset 125,100, screen scr_num 0, 1st monitor): `-screen 0 1280x720+125+100@1`  
      ```bash
      $ sudo apt-get install -y xfce4 xfce4-session xfce4-terminal
      #$ echo "export DISPLAY=localhost:0" >> ~/.bashrc
      #$ echo "export DISPLAY=:0.0" >> ~/.bashrc
      $ echo "export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2; exit;}'):0.0" >> ~/.bashrc
      $ echo "export LIBGL_ALWAYS_INDIRECT=1" >> ~/.bashrc
      
      $ vi ~/.bashrc
      
      DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2; exit;}')
      if [ "x$DISPLAY" == "x" ]; then DISPLAY=172.18.4.33:0.0; else DISPLAY=$DISPLAY:0.0; fi
      export DISPLAY=$DISPLAY
      export LIBGL_ALWAYS_INDIRECT=1
      ```
2. install [VcXSrv](https://sourceforge.net/projects/vcxsrv/) on Windows
    + WSL2: use XLaunch
    + over LAN: use VNC Viewer
3. launch `VcXSrv`
    + Select display settings: One large window
    + Extra settings: Disable access control
4. execute `startxfce4` on WSL

----
## References

[Official: WSL 2 的安裝指示](https://docs.microsoft.com/zh-tw/windows/wsl/wsl2-install)  
[Official: 建立和更新 WSL 散發套件的使用者帳戶](https://docs.microsoft.com/zh-tw/windows/wsl/user-support)  
[Official: Windows interoperability with Linux](https://docs.microsoft.com/zh-tw/windows/wsl/interop)  
[Official: WSL 2 Settings](https://docs.microsoft.com/en-us/windows/wsl/wsl-config#wsl-2-settings)  
[Official: Troubleshooting Windows Subsystem for Linux](https://docs.microsoft.com/zh-tw/windows/wsl/troubleshooting)
