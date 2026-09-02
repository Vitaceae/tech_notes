---
title: "Linux Debugging History"
tags: [linux, debugging]
draft: false
---

# Linux Debugging History

## Check if RTSP Server is Running

```bash
$ netstat -tuln | grep 554
tcp        0      0 0.0.0.0:554             0.0.0.0:*               LISTEN      
tcp        0      0 :::554                  :::*                    LISTEN      

$ fuser 554/tcp
800 

$ ps aux|grep 800
root         800  ...
```

## SO_REUSEPORT not supported

+ setsockopt() 回報錯誤，不支援 SO_REUSEPORT
+ EOPNOTSUPP (Operation not supported)

確認 kernel 是否支援 SO_REUSEPORT

```bash
# IPv6 socket test
python3 -c "import socket, struct; s = socket.socket(socket.AF_INET6); s.setsockopt(1, 15, struct.pack('i', 1)); print('OK')"

# UDP socket test
python3 -c "import socket, struct; s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM); s.setsockopt(1, 15, struct.pack('i', 1)); print('OK')"

# raw socket test
python3 -c "import socket, struct; s = socket.socket(socket.AF_INET, socket.SOCK_RAW, socket.IPPROTO_RAW); s.setsockopt(1, 15, struct.pack('i', 1)); print('OK')"
```

使用 strace 檢查系統呼叫

```bash
$ exec strace -f -tt -T -e trace=socket,setsockopt,bind,close -o /tmp/socket-test.log foo

# read the log
... socket(AF_UNIX, SOCK_STREAM, 0) = 42 <0.000032>
... setsockopt(42, SOL_SOCKET, SO_REUSEPORT, [1], 4) = -1 EOPNOTSUPP (Operation not supported) <0.000026>
... setsockopt(42, SOL_SOCKET, SO_REUSEADDR, [1], 4) = 0 <0.000026>
... bind(42, {sa_family=AF_UNIX, sun_path="..."}, 110) = 0 <0.000057>
```

分析:  
1. socket(AF_UNIX, SOCK_STREAM...)  
  建立 unix domain socket
2. setsockopt(... SOL_SOCKET, SO_REUSEPORT, ...)
3. EOPNOTSUPP (Operation not supported)  
  AF_UNIX 不支援 SO_REUSEPORT，故回報 EOPNOTSUPP

kernel 報錯原因:   

```c
if (sa_family != AF_UNIX) {
    setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, ...);
}
```