#!/bin/bash

FIFO_DIR="/tmp/run"
FIFO="$FIFO_DIR/cuckoo.pid"
LOG_FILE="cuckoo.log"

log() {
    echo "$(date '+%Y-%m-%d %H:%M:%S') $1" >> "$LOG_FILE"
}

cleanup() {
    log "Shutdown!"
    [ -p "$FIFO" ] && rm -f "$FIFO"
    exit 0
}
trap cleanup SIGTERM SIGINT

mkdir -p "$FIFO_DIR" || exit 1
[ -p "$FIFO" ] && rm -f "$FIFO"
mkfifo "$FIFO" || exit 1

log "Startup!"

while true; do
    if read -r line < "$FIFO"; then
        if [[ "$line" =~ ^([^[]+)\[([0-9]+)\]:\ how\ much\ time\ do\ I\ have\?$ ]]; then
            NAME="${BASH_REMATCH[1]}"
            PID="${BASH_REMATCH[2]}"
            N=$(( RANDOM % 9 + 2 ))
            log "$NAME[$PID] $N"
            echo "$N" > "$FIFO"
        else
            continue
        fi
    fi
done

cleanup