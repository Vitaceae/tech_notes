---
title: "SSH into Remote WSL"
tags: [it, ssh]
draft: false
---

# SSH into Remote WSL

透過 SSH 連線登入遠端 Windows 主機的 WSL。

----
## Configure Port-Forwarding

在遠端主機上設定轉發埠

```powershell
# forward <public_ip>:4000 to <internal_ip>:22
> netsh interface portproxy add v4tov4 listenport=4000 listenaddress=0.0.0.0 connectport=22 connectaddress=<internal_ip>
```

在本地主機上測試連線進遠端 Windows

```powershell
> Test-NetConnection 10.175.177.149 -p 4000
```

----
## Configure SSH on Local Host

Generate SSH key

```bash
$ ssh-keygen -t ed25519

$ vi ~/.ssh/config

Host            <alias of remote host>
HostName        <remote host ip>
Port            4000
User            <remote WSL login name: mike>
AddKeysToAgent  yes
IdentitiesOnly  yes
IdentityFile    /c/Users/<NT User>/.ssh/id_ed25519
```

啟動 SSH agent 以動態選擇 ssh key

```bash
$ vi ~/.bashrc

SSH_ENV=$HOME/.ssh/environment

function start_agent {
    echo "Initialising new SSH agent..."
    /usr/bin/ssh-agent | sed 's/^echo/#echo/' > "${SSH_ENV}"
    echo succeeded
    chmod 600 "${SSH_ENV}"
    . "${SSH_ENV}" > /dev/null
    /usr/bin/ssh-add;
}
 
# Source SSH settings, if applicable
 
if [ -f "${SSH_ENV}" ]; then
    . ${SSH_ENV} > /dev/null
    #ps ${SSH_AGENT_PID} doesn't work under cygwin
    ps -ef | grep ${SSH_AGENT_PID} | grep ssh-agent$ > /dev/null || {
        start_agent;
    }
else
    start_agent;
fi
 
ssh-add ~/.ssh/id_ed25519
```

Restart to invoke the SSH daemon

```bash
$ source ~/.bashrc
```

----
## SSH into Remote WSL

```bash
$ ssh-copy-id mike@<host alias>
$ ssh <host alias>
```