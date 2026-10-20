#!/bin/sh

echo -e "\n---- Start Baseline Latency Measurement Test ----\n"
taskset -c 1 lat_syscall -W 5 -N 20 null
taskset -c 1 lat_syscall -W 5 -N 20 read
sleep 1

echo -e "\n---- Start Memory Allocation Throughput Test ----\n"
sysbench memory --memory-block-size=1K --memory-total-size=100M run
sleep 1

echo -e "\n---- Start CPU Overhead Profiling Test ----\n"
perf stat -e cache-misses,cache-references,instructions,cycles -r 5 -- sysbench memory run
sleep 1

echo -e "\n---- Start Interrupt and Real-Time Latency Test ----\n"
cyclictest -q -m -p 99 --policy=rr -i 1000 -l 10000
sleep 1

echo -e "\n---- Start Kernel MMIO/Register Access Latency Microbenchmark Test ----\n"
dd if=/dev/zero of=/tmp/mmap_testfile bs=1M count=1
lat_mmap -W 2 -N 50 -P 1 1M /tmp/mmap_testfile
sleep 1

echo -e "\n---- Start Extreme stress testing of POSIX shared memory and mmap ----\n"
python3 -c "
import mmap, os, time

# create a 3MB Frame Buffer(1080p)
buf_size = 1920 * 1080 * 3 // 2
with open('/tmp/fake_framebuf.raw', 'wb') as f:
    f.write(b'\x00' * buf_size)

f = open('/tmp/fake_framebuf.raw', 'r+b')

start = time.perf_counter()
for _ in range(5000):
    # Simulate camera driver mmap + unmap
    m = mmap.mmap(f.fileno(), buf_size, flags=mmap.MAP_SHARED, prot=mmap.PROT_READ|mmap.PROT_WRITE)
    m.close()
end = time.perf_counter()

f.close()
os.remove('/tmp/fake_framebuf.raw')
print(f'Total Time for 5000 mmap/unmap cycles: {(end - start)*1000:.2f} ms')
print(f'Average Syscall Latency: {((end - start) / 5000) * 1e6:.2f} us')
"

echo -e "\n---- Start Evaluation of system call and memory copy overhead ----\n"
# Measure the execution speed and throughput of basic system calls, such as getppid.
perf bench syscall basic -l 1000000
# Memory Read/Write and Copy Performance Test
perf bench mem memcpy -l 2000 -s 1MB
perf bench mem memset -l 2000 -s 1MB
# IPC and Context Switch Latency Testing
perf bench sched pipe -l 50000
sleep 1

echo -e "\n---- Start Loop Recording File Write Test ----\n"
fio --name=cam_write --ioengine=libaio --direct=1 --iodepth=8 --rw=write --bs=128k --size=30M --numjobs=2 --filename_format="fio_test_\$jobnum.dat" --runtime=60 --time_based --group_reporting
rm -f fio_test_*.dat cam_write.*
sleep 1

echo -e "\n---- Start System Boot Time Evaluation ----\n"
systemd-analyze
sleep 1

echo -e "\n---- Start Security Test ----\n"

function get_kptr_restrict() {
    cat /sys/module/kernel/parameters/kptr_restrict 2>/dev/null || sysctl -n kernel.kptr_restrict
}

function test_kptr() {
    local addr_sample=$(cat /proc/kallsyms | head -n 1 | awk '{print $1}')

    if [ "$addr_sample" = "0000000000000000" ] || [ "$addr_sample" = "00000000" ]; then
        echo "Kernel pointers are HIDDEN ($addr_sample)."
    else
        echo "Kernel pointers are EXPOSED ($addr_sample)!"
    fi
}

function get_dmesg_restrict() {
    sysctl -n kernel.dmesg_restrict 2>/dev/null
}

function test_dmesg_perm() {
    local perm=$(su -s /bin/sh nobody -c "dmesg" 2>&1)

    if echo "$perm" | grep -qI "Operation not permitted\|Permission denied"; then
        echo "Restrict non-root users from reading dmesg SUCCESS."
    else
        echo "Restrict non-root users from reading dmesg FAILED."
    fi
}

echo "current kptr_restrict: $(get_kptr_restrict)"
echo "current dmesg_restrict: $(get_dmesg_restrict)"
sysctl -w kernel.kptr_restrict=2
test_kptr
test_dmesg_perm

exit 0
