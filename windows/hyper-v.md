# Windows Hyper-V

Enable/Disable Hyper-V in Powershell

```powershell
# Enable Hyper-V
#   -All: install Hyper-V GUI Management Tools and Hyper-V Module for Windows PowerShell
> Enable-WindowsOptionalFeature -Online -FeatureName Microsoft-Hyper-V –All

# Disable Hyper-V
> Disable-WindowsOptionalFeature -Online -FeatureName Microsoft-Hyper-V-All
```

Enable/Disable Hyper-V in CMD

```cmd
:: Enable Hyper-V
> dism.exe /Online /Enable-Feature:Microsoft-Hyper-V /All

:: Disable Hyper-V
> dism.exe /Online /Disable-Feature:Microsoft-Hyper-V-All
```