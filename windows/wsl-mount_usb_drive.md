# WSL Mount USB Drive

Windows 掛載 USB 並傳遞給 WSL 的 Linux 環境，以便直接存取 USB 裝置

```powershell
# Install USBIPD
PS C:\> winget install --interactive --exact dorssel.usbipd-win

# List available USB devices
PS C:\> usbipd list
Connected:
BUSID  VID:PID    DEVICE                                                        STATE
2-13   125f:dd35  USB Mass Storage Device                                       Not shared

# Bind USB device
PS C:\> usbipd bind --busid 2-13
PS C:\> usbipd list
Connected:
BUSID  VID:PID    DEVICE                                                        STATE
2-13   125f:dd35  USB Mass Storage Device                                       Shared

# Attach USB device
PS C:\> usbipd attach --wsl --busid 2-13
usbipd: info: Using WSL distribution 'Ubuntu' to attach; the device will be available in all WSL 2 distributions.
usbipd: info: Loading vhci_hcd module.
usbipd: info: Detected networking mode 'nat'.
usbipd: info: Using IP address 172.19.96.1 to reach the host.

# Detach USB device
PS C:\> usbipd detach --busid 2-13
```

現在 WSL 可直接存取 USB 裝置

```bash
$ lsusb
Bus 002 Device 002: ID 125f:dd35 A-DATA Technology Co., Ltd. ADATA USB Flash Drive
```
