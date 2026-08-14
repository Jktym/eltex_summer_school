#!/bin/bash

# Имя вашего исполняемого файла (если вы компилировали в main, замените на ./main)
EXE="./cp_files"

# Цвета для красивого вывода
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Проверка наличия исполняемого файла
if [ ! -f "$EXE" ]; then
    echo -e "${RED}Ошибка: Исполняемый файл $EXE не найден!${NC}"
    echo "Скомпилируйте программу: gcc main.c copy_module.c -o cp_files"
    exit 1
fi

echo -e "${YELLOW}=== Подготовка к тестированию ===${NC}"
echo "Создаем тестовые текстовые файлы..."
echo "Это содержимое первого тестового файла." > test1.txt
echo "А это второй файл. Он немного длиннее." > test2.txt

# Функция-обертка для запуска тестов
run_test() {
    local test_name="$1"
    local command="$2"
    
    echo -e "\n${YELLOW}--- Тест: $test_name ---${NC}"
    echo "\$ $command"
    
    # Выполняем команду
    eval $command
    
    # Проверяем код возврата (0 - успех, не 0 - ошибка)
    local status=$?
    if [ $status -eq 0 ]; then
        echo -e "Статус завершения: ${GREEN}$status (Успех)${NC}"
    else
        echo -e "Статус завершения: ${RED}$status (Ошибка)${NC}"
    fi
}

echo -e "\n${YELLOW}=========================================${NC}"
echo -e "${YELLOW}       БЛОК 1: НЕКОРРЕКТНЫЕ ДАННЫЕ       ${NC}"
echo -e "${YELLOW}=========================================${NC}"

run_test "Запуск вообще без аргументов" "$EXE"
run_test "Запуск с неизвестным флагом" "$EXE -z test1.txt"
run_test "Флаг -p, но не указано имя канала и файлы" "$EXE -p"
run_test "Флаг -p, указано имя канала, но нет файлов" "$EXE -p my_pipe"
run_test "Попытка скопировать несуществующий файл" "$EXE missing_file.txt"


echo -e "\n${YELLOW}=========================================${NC}"
echo -e "${YELLOW}        БЛОК 2: КОРРЕКТНЫЕ ДАННЫЕ        ${NC}"
echo -e "${YELLOW}=========================================${NC}"

run_test "Копирование одного файла (безымянный канал)" "$EXE test1.txt"
run_test "Копирование нескольких файлов (безымянный канал)" "$EXE test1.txt test2.txt"
run_test "Копирование одного файла (именованный канал)" "$EXE -p my_fifo test1.txt"
run_test "Копирование нескольких файлов (именованный канал)" "$EXE -p my_fifo test1.txt test2.txt"


echo -e "\n${YELLOW}=== Проверка результатов ===${NC}"
echo "Список созданных копий (должны быть test1.txt.copy и test2.txt.copy):"
ls -l *.copy 2>/dev/null


echo -e "\n${YELLOW}=== Очистка тестовой среды ===${NC}"
echo "Удаляем созданные файлы и копии..."
rm -f test1.txt test2.txt test1.txt.copy test2.txt.copy
# На случай, если канал не удалился сам (при падении программы)
rm -f my_fifo 

echo -e "${GREEN}Тестирование завершено!${NC}"