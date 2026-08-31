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
        N=$(( RANDOM % 9 + 2 ))
        
        log "$line -> $N"
        
        echo "$N" > "$FIFO"
    fi
done

cleanup