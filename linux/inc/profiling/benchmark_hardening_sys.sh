#!/bin/sh

DURATION=60 #sec
INTERVAL=1  #sec
LOG_FILE="sys_perf.csv"

echo "timestamp,user_pct,sys_pct,idle_pct,iowait_pct,mem_free_kb,ctx_switches" > "$LOG_FILE"
echo "---- Start logging system steady state data ----"

# Read /proc/stat line 1 (cpu).
get_cpu_times() {
    set -- $(grep '^cpu ' /proc/stat)
    # $2: user, $3: nice, $4: system, $5: idle, $6: iowait, $7: irq, $8: softirq
    USER=$2; NICE=$3; SYS=$4; IDLE=$5; IOWAIT=$6; IRQ=$7; SOFTIRQ=$8
    TOTAL=$((USER + NICE + SYS + IDLE + IOWAIT + IRQ + SOFTIRQ))
    SYS_TOTAL=$((SYS + IRQ + SOFTIRQ))
    echo "$USER $SYS_TOTAL $IDLE $IOWAIT $TOTAL"
}

# Read Context Switches
get_ctxt() {
    awk '/ctxt/ {print $2}' /proc/stat
}

# Initial sampling point
read PREV_USER PREV_SYS PREV_IDLE PREV_IOWAIT PREV_TOTAL <<EOF
$(get_cpu_times)
EOF
PREV_CTXT=$(get_ctxt)

ELAPSED=0
while [ $ELAPSED -lt $DURATION ]; do
    sleep $INTERVAL

    # Current sampling point
    read CURR_USER CURR_SYS CURR_IDLE CURR_IOWAIT CURR_TOTAL <<EOF
$(get_cpu_times)
EOF
    CURR_CTXT=$(get_ctxt)

    # Calculate time delta
    DIFF_TOTAL=$((CURR_TOTAL - PREV_TOTAL))
    if [ $DIFF_TOTAL -eq 0 ]; then
        DIFF_TOTAL=1;
    fi

    DIFF_USER=$((CURR_USER - PREV_USER))
    DIFF_SYS=$((CURR_SYS - PREV_SYS))
    DIFF_IDLE=$((CURR_IDLE - PREV_IDLE))
    DIFF_IOWAIT=$((CURR_IOWAIT - PREV_IOWAIT))

    # Calculate percentage
    PCT_USER=$(( (DIFF_USER * 100) / DIFF_TOTAL ))
    PCT_SYS=$(( (DIFF_SYS * 100) / DIFF_TOTAL ))
    PCT_IDLE=$(( (DIFF_IDLE * 100) / DIFF_TOTAL ))
    PCT_IOWAIT=$(( (DIFF_IOWAIT * 100) / DIFF_TOTAL ))

    # Calculate context switch diff and remaining memory
    DIFF_CTXT=$((CURR_CTXT - PREV_CTXT))
    MEM_FREE=$(awk '/MemFree:/ {print $2}' /proc/meminfo)
    TIMESTAMP=$(date +%H:%M:%S)

    echo "$TIMESTAMP,$PCT_USER,$PCT_SYS,$PCT_IDLE,$PCT_IOWAIT,$MEM_FREE,$DIFF_CTXT" >> "$LOG_FILE"

    PREV_USER=$CURR_USER
    PREV_SYS=$CURR_SYS
    PREV_IDLE=$CURR_IDLE
    PREV_IOWAIT=$CURR_IOWAIT
    PREV_TOTAL=$CURR_TOTAL
    PREV_CTXT=$CURR_CTXT

    ELAPSED=$((ELAPSED + INTERVAL))
done

echo "---- Stop logging ----"

awk -F',' 'NR>1 {
    us+=$2; sys+=$3; id+=$4; wa+=$5; ctxt+=$7; count++
} END {
    print "\n-------------------------------------------------------"
    printf "Avg. User CPU   (us)     : %d%%\n", us/count
    printf "Avg. Kernel CPU (sy)     : %d%%\n", sys/count
    printf "Avg. CPU Idle   (id)     : %d%%\n", id/count
    printf "Avg. I/O Wait   (wa)     : %d%%\n", wa/count
    printf "Avg. Context Switch/sec  : %d times/s\n", ctxt/count
    print "-------------------------------------------------------"
}' "$LOG_FILE"

exit 0