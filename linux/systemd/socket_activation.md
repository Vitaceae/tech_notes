---
title: "Socket Activation"
tags: [linux, systemd, socket]
draft: false
---

# Socket Activation

由 systemd 監聽通訊埠並觸發對應服務，適用並行機制

----
## Activation Flow

![systemd socket activation sequence diagram](systemd-socket-activation.png)

1. systemd 監聽指定的 port，對應的服務尚未啟動完成
2. systemd 收到 client request 後啟動對應的服務；若服務宣告 `WantedBy=multi-user.target`，可在觸發前啟動服務。
3. 服務從 systemd 獲得 fd 後執行 accept() 建立連接

----
## Example: custom service with socket activation

/etc/systemd/system/foo.socket

```bash
[Unit]
Description=Foo Socket

[Socket]
ListenStream=8080

[Install]
WantedBy=sockets.target
```

/etc/systemd/system/foo.service

```bash
[Unit]
Description=Foo Service
Requires=foo.socket
After=foo.socket

[Service]
Type=simple
Restart=always
ExecStart=/usr/sbin/foo
```

/usr/bin/foo  
+ 檢查 fd 常用: sd_listen_fds(), sd_is_socket(), sd_is_socket_inet(), sd_is_socket_unix()
+ SD_LISTEN_FDS_START 一般為 3，預設 fd 0=stdin, 1=stdout, 2=stderr

```c
    int fd;

    // check socket fd from systemd
    int n = sd_listen_fds(0); 

    if (n > 1) {
        // error: too much FD
        exit(1);
    } else if (n == 1) {
        // fd from systemd socket activation
        fd = SD_LISTEN_FDS_START + 0; 
    } else {
        // no fd from systemd; manually create fd, call bind() and listen()
        fd = create_and_bind_socket(8080);
        listen(fd, 10);
    }

    while (1) {
        int client_fd = accept(fd, ...);
        ...
    }
```

----
## Server Activation

1. systemd 預先為服務建立 TCP socket port 8080  
2. 服務呼叫 sd_listen_fds() 獲得 systemd 預建立的 fd (數量 n > 0)  
3. 從 fd=3 開始 poll 直到 3+n，呼叫 sd_is_socket_inet() 檢查該 fd 是否為 tcp socket port 8080

```c
#include <systemd/sd-daemon.h>

    int last_handle = SD_LISTEN_FDS_START + sd_listen_fds(0);
    int handle;

    for (handle = SD_LISTEN_FDS_START; handle < last_handle; ++handle) {
        //IPv4 TCP socket listening on port 8080
        if (sd_is_socket_inet(handle, AF_INET, SOCK_STREAM, 1, (uint16_t)8080) > 0) {
            break;
        }
        //accept(...);
}
```

[Demo: Server Activation](inc/demo-server_activation)  

+ systemd journal log: sd_journal_print()
+ 若 systemd 未傳入 socket fd，手動建立，監聽指定 port 8080
+ 使用 accept() 處理 client 連接請求
+ 每次處理一個 client request，讀取 client message 後輸出到 journal 並回傳 hello

```bash
# 
$ sudo apt install libsystemd-dev
$ sudo gcc -o /usr/local/bin/foo foo.c -lsystemd

$ sudo cp foo.socket /etc/systemd/system/foo.socket
$ sudo cp foo.service /etc/systemd/system/foo.service

$ sudo systemctl start foo.socket
$ sudo systemctl enable foo.socket

# 確認 port 8080 正被監聽
$ ss -ltnp
State   Recv-Q  Send-Q  Local Address:Port  Peer Address:Port   Process
LISTEN  0       4096        127.0.0.1:8080       0.0.0.0:*
...

# 持續查看 journal 輸出
$ sudo journalctl -u foo -f
```

模擬客戶端: 連接 localhost port 8080 發送訊息給 server，觸發前述 journal output

```bash
$ gcc -o client client.c
$ ./client
```

中止服務

```
# 僅停止服務，port 8080 仍被 systemd 監聽
$ sudo systemctl stop foo
Stopping 'foo.service', but its triggering units are still active:
foo.socket

$ ss -lntp|grep 8080
LISTEN 0      4096        127.0.0.1:8080       0.0.0.0:*

$ sudo systemctl stop foo.socket
$ ss -lntp|grep 8080
```

----
## Activation with accept()

+ .socket 檔案，設定 `Accept=no`(預設 yes)
    + systemd 負責建立 socket, bind, listen, accept，將 fd 返回給新啟動的服務 instance
+ 對應的服務檔案名改為 `foo@.service`
    + 須設定 `StandardInput=socket` 和 `StandardOutput=socket` 綁定並映射收到的 fd
    + connect, recv 重新導向到 STDIN_FILENO
    + send 被重新導向到 STDOUT_FILENO
    + 不須處理 listen() 及 accept()
+ 用於實現並行 concurrent 機制，不須在服務中實現多執行緒 multithreading 或多行程 multiprocessing

[Demo: Server Activation with accept()](inc/demo-server_activation_with_accept)  

啟動兩個 client，查看 server session 的 log

```bash
$ journalctl -xe | grep -i foo
```

----
## References

[systemd-socket-activate](https://www.freedesktop.org/software/systemd/man/latest/systemd-socket-activate.html)  
[Start a systemd Service on the First Connection](https://labs.iximiuz.com/challenges/systemd-socket-activate-listen-stream-inet)  
[systemd for Developers I](https://0pointer.de/blog/projects/socket-activation.html)  
[systemd for Developers II](https://0pointer.de/blog/projects/socket-activation2.html)  