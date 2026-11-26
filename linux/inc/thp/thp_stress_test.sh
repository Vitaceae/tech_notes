#!/bin/bash

THP=/sys/kernel/mm/transparent_hugepage
OUT=/tmp/thp-test
DURATION=60

mkdir -p "$OUT"

# Check THP interface
if [ ! -d "$THP" ]; then
    echo "ERROR: THP not supported"
    exit 1
fi

echo "---- THP Configuration ----"

cat "$THP/enabled"
cat "$THP/defrag"

for d in "$THP"/hugepages-*; do
    [ -d "$d" ] || continue
    echo "[$d]"
    cat "$d/enabled"
done

# Capture statistics
capture_stats()
{
    local name=$1

    grep -E '^(thp_|compact_)' /proc/vmstat \
        > "$OUT/vmstat.$name"

    cat /proc/meminfo > "$OUT/meminfo.$name"

    for d in "$THP"/hugepages-*; do
        [ -d "$d/stats" ] || continue

        for f in "$d"/stats/*; do
            [ -f "$f" ] || continue
            echo "$f: $(cat "$f")"
        done
    done > "$OUT/mthp_stats.$name"
}

echo "---- Baseline ----"
capture_stats before

echo "---- Start stress test ----"

stress -q \
    --vm 1 \
    --vm-bytes 64M \
    --vm-stride 4096 \
    --vm-keep \
    --timeout 60s \
    > "$OUT/stress.log" 2>&1 &

STRESS_PID=$!

echo "stress PID: $STRESS_PID"

# Wait for worker allocation
sleep 3

echo "---- Process information ----"

ps -ef | grep '[s]tress'

# Capture memory mapping of stress worker
for PID in $(pgrep -x stress); do
    if [ -r "/proc/$PID/smaps_rollup" ]; then
        cat "/proc/$PID/smaps_rollup" \
            > "$OUT/smaps_rollup.$PID"
    fi
done

echo "---- Runtime statistics ----"
capture_stats during

wait "$STRESS_PID"

echo "---- Final statistics ----"
capture_stats after

echo "---- vmstat difference ----"
diff -u "$OUT/vmstat.before" \
        "$OUT/vmstat.after" || true

echo "---- mTHP statistics difference ----"
diff -u "$OUT/mthp_stats.before" \
        "$OUT/mthp_stats.after" || true

echo "---- Stress output ----"
cat "$OUT/stress.log"

echo "Report has been saved to $OUT"