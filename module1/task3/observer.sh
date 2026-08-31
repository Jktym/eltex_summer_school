#!/bin/bash

CONF="observer.conf"
LOG="observer.log"

while read line; do
    if [ -z "$line" ]; then
        continue
    fi

    found=0
    
    for cmd_file in /proc/[0-9]*/cmdline; do
        if grep -a -q "$line" "$cmd_file" 2>/dev/null; then
            found=1
            break
        fi
    done

    if [ "$found" -eq 0 ]; then
        nohup ./"$line" >> /dev/null 2>&1 &
        echo "$(date '+%Y-%m-%d %H:%M:%S') Скрипт $line перезапущен" >> "$LOG"
    fi
done < "$CONF"