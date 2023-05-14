#!/bin/bash

# Именованные каналы
nav_data="/tmp/nav_data"
hcsr04_data="/tmp/range_finder"

# Аргументы для программ nav_module и range_finder
nav_module_args="-g /tmp/nav_data"
range_finder_args="400 /tmp/range_finder"

# Создание файла для записи результата
result_file="result.txt"
touch $result_file

# Запуск программы nav_module с аргументами
./nav_data_stub $nav_module_args &

# Запуск программы range_finder с аргументами
./range_finder_stub $range_finder_args &

./combiner

# Бесконечный цикл
while true
do
    # Считывание навигационных параметров из именованного канала
    read nav_params < $nav_data
    echo "Nav Params: $nav_params"

    # Считывание данных дальномера из именованного канала
    read hcsr04_params < $hcsr04_data
    echo "HCSR04 Params: $hcsr04_params"

    # Разделение навигационных параметров по пробелу
    nav_arr=($nav_params)

    # Получение временной отметки
    timestamp=$(date +%s.%N)

    # Рассчет высоты по формуле H = t * 340 / 2
    # h=$(echo "scale=2; ${hcsr04_params}*340/2" | bc)

    # Формирование пакета навигационных параметров с высотой и временной отметкой
    packet="$hcsr04_params $nav_params $timestamp"

    # Запись пакета в файл
    echo $packet >> $result_file

    sleep 0.1

done
