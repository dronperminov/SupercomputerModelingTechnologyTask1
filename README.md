# SupercomputerModelingTechnologyTask1
Реализация первого практического задания в рамках курса "Суперкомпьютерное моделирование и технологии"

Реализована параллельная MPI-программа, принимающая от 0 до 9 аргументов (доступно при запуске с флагом --help):

* `-e` – необходимая точность вычисления интеграла (по умолчанию 1e-3);
* `-n` – количество точек, отдаваемых каждому процессу (по умолчанию 1000);
* `-d` – флаг отладки для вывода дополнительной информации (по умолчанию не используется);
* `-xmin` – нижняя граница изменения переменной x в параллелипипеде (по умолчанию 0);
* `-xmax` – верхняя граница изменения переменной x в параллелипипеде (по умолчанию 1);
* `-ymin` – нижняя граница изменения переменной y в параллелипипеде (по умолчанию 0);
* `-ymax` – верхняя граница изменения переменной y в параллелипипеде (по умолчанию 1);
* `-zmin` – нижняя граница изменения переменной z в параллелипипеде (по умолчанию 0);
* `-zmax` – верхняя граница изменения переменной z в параллелипипеде (по умолчанию 1);

При запуске программы все процессы считывают переданные аргументы, проверяя их на корректность, после чего в случае успешной проверки запускают основной цикл вычислений.
В качестве параллельной реализации используется парадигма «мастер–рабочие»: один из процессов («мастер») генерирует случайные точки и передаёт каждому из остальных процессов («рабочих») отдельный, предназначенный для него, набор сгенерированных случайных точек. Все процессы – рабочие вычисляют свою часть суммы. Затем вычисляется общая сумма с помощью операции редукции и итоговый интеграл.

Полученное значение интеграла сравнивается с заранее заданным эталоном и в случае достижения требуемой точности процессам рабочим передаётся флаг, сигнализирующий об окончании вычислений и завершении работы. В противном случае генерируется новая порция точек и процесс вычисления повторяется.

## Сборка программы
Сборка осуществляется путём выполнения команды `make` из директории с программой. Для отправки на учебные кластеры используются цели `submit-polus` и `submit-bluegene`:

### Пример сборки:
```bash
> make
> make submit-polus
> make submit-bluegene
```

## Пример работы программы
```
Processors: 16
Eps: 1e-06
Points per process: 1000
Dimensions: [0, 1] x [0, 1] x [0, 1]
Volume: 1
Loops: 129
Total number of points: 2064000
Target integrate value: 0.00274725
Calculated integrate value: 0.0027475
Error: 2.48997e-07
Max time: 0.0163059
Min time: 0.0162583
Average time: 0.0162845
```

## Таблица с результатами расчётов для системы Bluegene (N = 1000, мастер-рабочий)

| Точность (ε) | Число MPI процессов | Время работы программы (с) | Ускорение |    Ошибка   |
|          :-: |                 :-: |                        :-: |       :-: |         :-: |
|         1e-4 |                   2 |                   0.033324 |         1 | 5.69829e-05 |
|         1e-4 |                   4 |                   0.029836 |     1.116 | 5.00716e-05 |
|         1e-4 |                  16 |                   0.051855 |     0.642 | 6.17547e-05 |
|         1e-4 |                  64 |                   0.082581 |     0.403 | 5.75076e-05 |
|         2e-5 |                   2 |                   0.137660 |         1 |  1.2201e-05 |
|         2e-5 |                   4 |                   0.524361 |     0.262 | 1.44155e-05 |
|         2e-5 |                  16 |                   0.167715 |     0.820 | 1.04416e-05 |
|         2e-5 |                  64 |                   0.329309 |     0.418 | 1.27367e-05 |
|         8e-6 |                   2 |                   0.130379 |         1 | 4.75472e-06 |
|         8e-6 |                   4 |                   1.850610 |      0.07 | 5.02529e-06 |
|         8e-6 |                  16 |                   0.375042 |      0.34 | 4.77424e-06 |
|         8e-6 |                  64 |                   2.159940 |      0.06 | 4.32039e-06 |


## Таблица с результатами расчётов для системы Polus (N = 1000, мастер-рабочий)

| Точность (ε) | Число MPI процессов | Время работы программы (с) | Ускорение |    Ошибка   |
|          :-: |                 :-: |                        :-: |       :-: |         :-: |
|         3e-5 |                   2 |                   0.013644 |         1 | 1.68166e-05 |
|         3e-5 |                   4 |                   0.015771 |     0.865 | 1.62863e-05 |
|         3e-5 |                  16 |                   0.020990 |     0.650 | 1.75938e-05 |
|         3e-5 |                  64 |                   0.026139 |     0.521 | 1.53217e-05 |
|         5e-6 |                   2 |                   0.020224 |         1 | 1.89184e-06 |
|         5e-6 |                   4 |                   0.383937 |     0.052 | 3.17904e-06 |
|         5e-6 |                  16 |                   0.239609 |     0.084 | 3.27953e-06 |
|         5e-6 |                  64 |                   0.277693 |     0.072 | 2.66778e-06 |
|       1.5e-6 |                   2 |                   0.020058 |         1 | 1.05166e-06 |
|       1.5e-6 |                   4 |                   1.468130 |     0.013 | 7.35179e-07 |
|       1.5e-6 |                  16 |                   1.224790 |     0.016 | 8.43032e-07 |
|       1.5e-6 |                  64 |                   2.830310 |     0.007 | 8.21990e-07 |


## Таблица с результатами расчётов для системы Bluegene (N = 1000, обычная схема)

| Точность (ε) | Число MPI процессов | Время работы программы (с) | Ускорение |    Ошибка   |
|          :-: |                 :-: |                        :-: |       :-: |         :-: |
|         1e-4 |                   1 |                   0.013390 |         1 | 5.62939e-05 |
|         1e-4 |                   4 |                   0.006371 |     2.102 | 5.82478e-05 |
|         1e-4 |                  16 |                   0.002604 |     5.142 | 5.84753e-05 |
|         1e-4 |                  64 |                   0.001725 |     7.762 | 5.51759e-05 |
|         2e-5 |                   1 |                   0.117756 |         1 | 1.18561e-05 |
|         2e-5 |                   4 |                   0.048907 |     2.407 | 1.15702e-05 |
|         2e-5 |                  16 |                   0.011841 |     9.944 | 1.16796e-05 |
|         2e-5 |                  64 |                   0.009600 |    12.266 | 1.17729e-05 |
|         8e-6 |                   1 |                   0.354141 |         1 | 4.39475e-06 |
|         8e-6 |                   4 |                   0.446997 |     0.792 | 5.23333e-06 |
|         8e-6 |                  16 |                   0.054583 |     6.488 | 4.63279e-06 |
|         8e-6 |                  64 |                   0.036026 |     9.830 | 5.20942e-06 |


## Таблица с результатами расчётов для системы Polus (N = 1000, обычная схема)

| Точность (ε) | Число MPI процессов | Время работы программы (с) | Ускорение |    Ошибка   |
|          :-: |                 :-: |                        :-: |       :-: |         :-: |
|         3e-5 |                   1 |                  0.0071149 |         1 | 1.83298e-05 |
|         3e-5 |                   4 |                  0.0046659 |     1.524 | 1.94462e-05 |
|         3e-5 |                  16 |                  0.0030029 |     2.369 | 1.61581e-05 |
|         3e-5 |                  64 |                  0.0027978 |     2.543 | 1.81731e-05 |
|         5e-6 |                   1 |                  0.0724638 |         1 | 2.53375e-06 |
|         5e-6 |                   4 |                  0.0337195 |     2.149 | 2.57642e-06 |
|         5e-6 |                  16 |                  0.0133617 |     5.423 | 2.84998e-06 |
|         5e-6 |                  64 |                  0.0136074 |     5.325 | 3.15469e-06 |
|       1.5e-6 |                   1 |                  0.7495280 |         1 | 7.92767e-07 |
|       1.5e-6 |                   4 |                  0.4252140 |     1.762 | 7.19423e-07 |
|       1.5e-6 |                  16 |                  0.0510382 |    14.685 | 9.54345e-07 |
|       1.5e-6 |                  64 |                  0.0596887 |    12.557 | 9.14685e-07 |
