#!/bin/bash

SCRIPT_NAME=$(basename "$0")

if [ "$SCRIPT_NAME" == "template_task.sh" ]; then
    echo "я бригадир, сам не работаю"
    exit 0
fi

LOG_FILE="report_${SCRIPT_NAME}.log"
MY_PID=$$
DATE_TIME=$(date '+%Y-%m-%d %H:%M:%S')

echo "$DATE_TIME [$MY_PID] Скрипт запущен" >> "$LOG_FILE"

FIFO="/tmp/run/cuckoo.pid"
LOCK_FILE="/tmp/run/cuckoo.lock"

if [ ! -p "$FIFO" ]; then
    echo "$DATE_TIME [$MY_PID] Ошибка: канал cuckoo не найден. Выхожу." >> "$LOG_FILE"
    exit 1
fi

exec 200>"$LOCK_FILE"

flock 200

echo "${SCRIPT_NAME}[${MY_PID}]: how much time do I have?" > "$FIFO"
read N < "$FIFO"

flock -u 200
exec 200>&-

if [ -z "$N" ]; then
    echo "$DATE_TIME [$MY_PID] Ошибка: получен пустой ответ от сервера." >> "$LOG_FILE"
    N=1
fi

sleep "$N"

DATE_TIME_END=$(date '+%Y-%m-%d %H:%M:%S')
echo "$DATE_TIME_END [$MY_PID] Скрипт завершился, работал $N секунд." >> "$LOG_FILE"