---
title: "SAMBA on Remote WSL"
tags: [it, samba]
draft: false
---

# SAMBA on Remote WSL

設定並連線遠端 Windows 主機的 WSL SAMBA 服務，忽略防火牆設定。

----
## Set up SAMBA service on Remote Host

在 WSL 環境開啟 SAMBA 服務

```bash
# Install dependencies
$ sudo apt install samba smbclient samba-common

# Start SMB services
$ sudo systemctl start smbd; sudo systemctl enable smbd
$ sudo systemctl start nmbd; sudo systemctl enable nmbd

$ sudo vi /etc/samba/smb.conf

[homes]
    comment = Home Directories
    valid users = %S, %D%w%S
    browseable = yes
    read only = no
    create mask = 0700
    directory mask = 0700
    inherit acls = Yes
    follow symlinks = yes
    wide links = yes

# add new user 'mike' to samba database
$ sudo usermod -a -G smbgrp mike
$ sudo smbpasswd -a mike
# enter password

$ sudo systemctl restart smbd; sudo systemctl restart nmbd

# per-user configuration
$ sudo vi /etc/samba/smb.conf

browseable = no
include = /etc/samba/per-user/%u.conf

$ sudo mkdir /etc/samba/per-user
$ sudo vi /etc/samba/per-user/mike.conf

browseable = yes

# test connection locally
$ smbclient //localhost -U=mike
```

----
## Set up Port-Forwarding on Remote Host

在 Windows 環境設定轉發連接埠

```powershell
# forward <public_ip>:4100 to <internal_ip>:445
> netsh interface portproxy add v4tov4 listenport=4100 listenaddress=0.0.0.0 connectport=445 connectaddress=<internal_ip>
```

----
## Mount Remote Host's Home on Local WSL

```bash
# samba user: mike
# linux user: mike
# test connection on local WSL
$ smbclient //<public_ip>/mike -U=mike --port=4100

# mount remote home directory to /mnt/smb_pc
$ sudo apt install cifs-utils
$ sudo mkdir /mnt/smb_pc
$ sudo mount -t cifs //<public_ip>/mike /mnt/smb_pc -o username=mike,password=<password>,port=4100,uid=mike
```
