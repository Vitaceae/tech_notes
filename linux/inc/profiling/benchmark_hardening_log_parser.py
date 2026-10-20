#!/usr/bin/env python3
# Usage: python benchmark_hardening_log_parser.py <baseline_log> <hardened_log>

import sys
import re

def parse_log(filepath):
    data = {}
    with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    # Latency/Syscalls
    m = re.search(r"Simple syscall:\s*([\d\.]+)\s*microseconds", content)
    if m: data['simple_syscall_us'] = float(m.group(1))

    m = re.search(r"Simple read:\s*([\d\.]+)\s*microseconds", content)
    if m: data['simple_read_us'] = float(m.group(1))

    # Memory Bandwidth
    m = re.search(r"100\.00 MiB transferred \(([\d\.]+)\s*MiB/sec\)", content)
    if m: data['sysbench_mem_bw_mib'] = float(m.group(1))

    # cache-misses, insn per cycle
    m = re.search(r"([\d\.]+)\s*%\s*of all cache refs", content)
    if m: data['cache_miss_pct'] = float(m.group(1))

    m = re.search(r"([\d\.]+)\s*insn per cycle", content)
    if m: data['ipc'] = float(m.group(1))

    # Cyclictest Latency
    m = re.search(r"Min:\s*(\d+)\s*Act:\s*(\d+)\s*Avg:\s*(\d+)\s*Max:\s*(\d+)", content)
    if m:
        data['cyclictest_min_us'] = int(m.group(1))
        data['cyclictest_avg_us'] = int(m.group(3))
        data['cyclictest_max_us'] = int(m.group(4))

    # lat_mmap (1MB)
    m = re.search(r"1\.000000\s+(\d+)", content)
    if m: data['lat_mmap_1mb_us'] = int(m.group(1))

    # Shared memory & mmap stress
    m = re.search(r"Average Syscall Latency:\s*([\d\.]+)\s*us", content)
    if m: data['shm_mmap_avg_us'] = float(m.group(1))

    # Perf bench
    m = re.search(r"""# Running 'syscall/basic' benchmark:.*?\s*([\d\.]+)\s*usecs/op""", content, re.DOTALL)
    if m: data['perf_syscall_usec'] = float(m.group(1))

    m = re.search(r"""# Running 'mem/memcpy' benchmark:.*?\s*([\d\.]+)\s*GB/sec""", content, re.DOTALL)
    if m: data['perf_memcpy_gb'] = float(m.group(1))

    m = re.search(r"""# Running 'mem/memset' benchmark:.*?\s*([\d\.]+)\s*GB/sec""", content, re.DOTALL)
    if m: data['perf_memset_gb'] = float(m.group(1))

    m = re.search(r"""# Running 'sched/pipe' benchmark:.*?\s*([\d\.]+)\s*usecs/op""", content, re.DOTALL)
    if m: data['perf_pipe_usec'] = float(m.group(1))

    # FIO Storage Write (Loop recording)
    m = re.search(r"""WRITE:\s*bw=([\d\.]+)\s*MiB/s""", content)
    if m: data['fio_write_bw_mib'] = float(m.group(1))

    m = re.search(r"""clat\s*\(msec\):.*avg=\s*([\d\.]+)""", content)
    if m: data['fio_clat_avg_us'] = float(m.group(1))

    # Boot Time
    m = re.search(r"""Startup finished in\s*([\d\.]+)s\s*\(kernel\)\s*\+\s*([\d\.]+)s\s*\(userspace\)\s*=\s*([\d\.]+)s""", content)
    if m:
        data['boot_kernel_s'] = float(m.group(1))
        data['boot_userspace_s'] = float(m.group(2))
        data['boot_total_s'] = float(m.group(3))

    # Security Test
    m = re.search(r"""Kernel pointers are\s+(HIDDEN|EXPOSED)""", content)
    if m: data['kptr_status'] = m.group(1)

    m = re.search(r"""Restrict non-root users from reading dmesg\s+(SUCCESS|FAILED)""", content)
    if m: data['dmesg_status'] = m.group(1)

    return data

def calculate_diff(base, hard, lower_is_better=True):
    if base is None or hard is None:
        return "N/A", "N/A"
    diff = hard - base
    pct = (diff / base) * 100 if base != 0 else 0.0
    
    if abs(pct) < 2.0:
        status = "Pass (<2% Delta)"
    else:
        if (lower_is_better and pct < 0) or (not lower_is_better and pct > 0):
            status = f"Improved ({pct:+.2f}%)"
        else:
            status = f"Degraded ({pct:+.2f}%)"
    return f"{pct:+.2f}%", status

def generate_report(base_data, hard_data):
    metrics = [
        ("Simple Syscall Latency", "simple_syscall_us", "us", True),
        ("Simple Read Latency", "simple_read_us", "us", True),
        ("Memory Bandwidth (Sysbench)", "sysbench_mem_bw_mib", "MiB/s", False),
        ("Instruction per Cycle (IPC)", "ipc", "insn/cyc", False),
        ("Cyclictest Max Latency", "cyclictest_max_us", "us", True),
        ("lat_mmap 1MB Latency", "lat_mmap_1mb_us", "us", True),
        ("5000 mmap/unmap Avg Latency", "shm_mmap_avg_us", "us", True),
        ("perf bench Syscall Latency", "perf_syscall_usec", "us/op", True),
        ("perf bench Memcpy Throughput", "perf_memcpy_gb", "GB/s", False),
        ("perf bench Memset Throughput", "perf_memset_gb", "GB/s", False),
        ("perf bench Pipe Context Switch", "perf_pipe_usec", "us/op", True),
        ("FIO Loop Write Bandwidth", "fio_write_bw_mib", "MiB/s", False),
        ("FIO Write Avg Latency (clat)", "fio_clat_avg_us", "us", True),
        ("Kernel Boot Time", "boot_kernel_s", "sec", True),
        ("Total Boot Time", "boot_total_s", "sec", True),
    ]

    print("+" + "-"*34 + "+" + "-"*16 + "+" + "-"*16 + "+" + "-"*12 + "+" + "-"*20 + "+")
    header = f"| {'Test Metric':<32} | {'Baseline':<14} | {'Hardened':<14} | {'Delta (%)':<10} | {'Assessment':<18} |"
    print(header)
    print("|" + "-"*34 + "|" + "-"*16 + "|" + "-"*16 + "|" + "-"*12 + "|" + "-"*20 + "|")

    for label, key, unit, lower_is_better in metrics:
        b_val = base_data.get(key)
        h_val = hard_data.get(key)
        
        b_str = f"{b_val:.2f} {unit}" if isinstance(b_val, float) else f"{b_val} {unit}" if b_val is not None else "N/A"
        h_str = f"{h_val:.2f} {unit}" if isinstance(h_val, float) else f"{h_val} {unit}" if h_val is not None else "N/A"

        pct_str, status = calculate_diff(b_val, h_val, lower_is_better)
        print(f"| {label:<32} | {b_str:<14} | {h_str:<14} | {pct_str:<10} | {status:<18} |")

    # Security Test
    print(f"| {"Kernel Pointer":<32} | {base_data.get('kptr_status', 'N/A'):<14} | {hard_data.get('kptr_status', 'N/A'):<14} | {"":<10} | {"":<18} |")
    print(f"| {"dmesg restrict":<32} | {base_data.get('dmesg_status', 'N/A'):<14} | {hard_data.get('dmesg_status', 'N/A'):<14} | {"":<10} | {"":<18} |")

    print("+" + "-"*34 + "+" + "-"*16 + "+" + "-"*16 + "+" + "-"*12 + "+" + "-"*20 + "+")

if __name__ == "__main__":
    base_log = sys.argv[1] if len(sys.argv) > 1 else 'test.log'
    hard_log = sys.argv[2] if len(sys.argv) > 2 else 'test.log'

    base_data = parse_log(base_log)
    hard_data = parse_log(hard_log)
    generate_report(base_data, hard_data)
