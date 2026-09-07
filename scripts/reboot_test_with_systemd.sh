#!/bin/bash
# strict mode: immediately stops execution and reports the error when an error occurs
set -euo pipefail

# --- Configuration ---
readonly WAIT_SECS=30
readonly WORK_DIR="/var/run/reboot-test"
readonly LOG_FILE="/var/log/reboot-test.log"
readonly STATE_FILE="${WORK_DIR}/state.json"
readonly SVC_NAME="reboot_test_with_systemd"

log() {
    echo "[$(date +'%F %T')] $*" | tee -a "$LOG_FILE";
}

cleanup() {
    log "Test completed. Cleaning up..."
    systemctl disable $SVC_NAME &>/dev/null || true
    rm -f "/etc/systemd/system/${SVC_NAME}.service" /usr/local/bin/reboot_test_with_systemd.sh
    rm -rf "$WORK_DIR"
    systemctl daemon-reload
    exit 0
}

setup_service() {
    cp "$0" /usr/local/bin/reboot_test_with_systemd.sh
    cat <<EOF > "/etc/systemd/system/${SVC_NAME}.service"
[Unit]
Description=Reboot Test Service
After=network.target

[Service]
Type=oneshot
ExecStart=/usr/local/bin/reboot_test_with_systemd.sh

[Install]
WantedBy=multi-user.target
EOF
    systemctl daemon-reload && systemctl enable $SVC_NAME
}

run_custom_tests() {
    log "Running custom test items..."

    # custom test 1
    # systemctl is-active --quiet docker || { log "FAIL: Docker inactive"; exit 1; }

    # custom test 2
    # /usr/bin/foo.sh >> "$LOG_FILE" 2>&1

    log "All tests passed."
}

[[ $EUID -eq 0 ]] || { echo -e "\e[31mError: Root privileges required.\e[0m"; exit 1; }

mkdir -p "$WORK_DIR" "$(dirname "$LOG_FILE")"

if [[ ! -f $STATE_FILE ]]; then
    [[ ${1:-} =~ ^[0-9]+$ ]] || { echo -e "\e[33mUsage: $0 <target_cycles>\e[0m"; exit 1; }
    echo "{\"curr\": 0, \"target\": $1}" > "$STATE_FILE"
    setup_service
    log "Initialized test. Target cycles: $1"
fi

run_custom_tests

curr=$(jq -r '.curr' "$STATE_FILE")
target=$(jq -r '.target' "$STATE_FILE")

if (( curr >= target )); then
    log "SUCCESS: Completed all $target reboot cycles."
    cleanup
fi

curr=$((curr + 1))
jq --argjson c "$curr" '.curr = $c' "$STATE_FILE" > "${STATE_FILE}.tmp" && mv "${STATE_FILE}.tmp" "$STATE_FILE"

log "Progress: [$curr/$target] - Rebooting in ${WAIT_SECS}s..."
sleep "$WAIT_SECS"
systemctl reboot