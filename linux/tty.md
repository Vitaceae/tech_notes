# Linux TTY Subsystem

----
# Commands

```bash
# get serial port Info
$ cat /proc/devices
$ cat /proc/tty/driver/serial
$ dmesg | grep tty

# get port parameters(baud, data bits, etc.)
$ busybox stty -F /dev/ttyS0
$ stty -F /dev/ttyS0 -a

# set port parameters
$ stty -F /dev/ttyS0 ispeed 115200 ospeed 115200 cs8
# alternative
$ setserial -g /dev/ttyS*

# listen to port x
$ cat /dev/ttySx

# tx/rx test
#   receiver session
$ cat /dev/ttySx
#   transmitter session
$ echo "test" >/dev/ttySx
```
