# RTL-style Pipeline Model

Учебный C++ проект для моделирования простой RTL-style pipeline-модели и проверки поведения после добавления задержки на один такт.

Проект имитирует базовую verification-задачу: есть комбинационная модель без задержки и pipeline-модель с задержкой. Программа сравнивает их выходы с учётом latency, `valid`, `reset` и выводит mismatch diagnostics при расхождении.

---

## Основная идея

Базовое вычисление:

```text
y = (a + b) * c
```

В проекте есть две модели:

- `CombinationModel` — комбинационная модель, выдаёт результат на том же такте;
- `PipelinedModel` — pipeline-модель, выдаёт результат на следующем такте.

Сравнение выполняется с учётом задержки:

```text
comb_outputs[i] сравнивается с pipe_outputs[i + 1]
```

То есть:

```text
input на cycle N -> output pipeline на cycle N + 1
```

---

## Поддерживаемые сигналы

Каждый входной такт содержит пять значений:

```text
reset valid a b c
```

Где:

- `reset` — сброс pipeline-регистров;
- `valid` — признак того, что входные данные настоящие;
- `a`, `b`, `c` — входные значения;
- `y` — выходное значение.

Если `reset = 1`, pipeline очищается, а выход считается невалидным:

```text
valid = 0
y = 0
```

---

## Что делает программа

Программа:

1. читает входной trace из файла;
2. валидирует формат входных данных;
3. прогоняет данные через `CombinationModel`;
4. прогоняет данные через `PipelinedModel`;
5. добавляет flush-такт для pipeline;
6. печатает таблицу по всем cycle;
7. сравнивает выходы моделей с учётом задержки `latency = 1`;
8. учитывает `reset` на output-cycle;
9. выводит `Check: OK` или `Check: FAILED`;
10. при ошибке выводит mismatch report.

---

## Формат input trace

Файл входных данных содержит по одному такту на строку:

```text
reset valid a b c
```

Пример корректного файла:

```text
1 0 0 0 0
0 1 2 3 4
0 1 1 5 2
0 0 9 9 9
0 1 3 4 5
1 0 0 0 0
0 1 10 2 3
0 0 0 0 0
```

Пустые строки допускаются и пропускаются.

---

## Валидация input trace

`trace_reader` проверяет:

- файл должен открываться;
- строка должна содержать ровно 5 параметров;
- все параметры должны быть числами;
- `reset` должен быть равен `0` или `1`;
- `valid` должен быть равен `0` или `1`;
- файл не должен быть пустым.

Примеры ошибок:

```text
Wrong number of parameters in line N
Not a number in line N
Wrong valid / reset argument in line N
File is empty
Couldn`t read file
```

---

## Mismatch diagnostics

Если модели расходятся, программа выводит отчёт об ошибке:

```text
Check: FAILED

Error in cycle - ...
On pipe cycle - ...
Expected valid - ...
Expected y - ...
Actual valid - ...
Actual y - ...
Reset occurred - true/false
Reason: ...
```

Отчёт помогает понять:

- на каком input-cycle возникло расхождение;
- на каком pipe-cycle ожидался результат;
- какой результат ожидался;
- какой результат был получен;
- повлиял ли `reset` на output-cycle.

---

## Структура проекта

```text
RTL-style-pipeline-model/
├── include/
│   ├── checker.h
│   ├── errors.h
│   ├── models.h
│   ├── samples.h
│   └── trace_reader.h
├── src/
│   ├── checker.cpp
│   ├── errors.cpp
│   ├── models.cpp
│   ├── samples.cpp
│   └── trace_reader.cpp
├── tests/
│   ├── valid_trace.txt
│   ├── bad_reset.txt
│   ├── bad_valid.txt
│   ├── bad_format_short.txt
│   ├── bad_format_long.txt
│   ├── not_a_number.txt
│   └── empty.txt
├── input.txt
├── main.cpp
├── Makefile
├── README.md
└── .gitignore
```

---

## Сборка

Проект собирается через `Makefile`.

```bash
make
```

или:

```bash
make all
```

В результате создаётся исполняемый файл:

```text
app
```

---

## Запуск

### Запуск с файлом по умолчанию

```bash
make run
```

По умолчанию используется файл:

```text
input.txt
```

### Запуск с произвольным trace-файлом

```bash
make run FILE=tests/valid_trace.txt
```

Примеры:

```bash
make run FILE=tests/bad_reset.txt
make run FILE=tests/not_a_number.txt
```

---

## Тестовые сценарии

В проекте есть набор входных trace-файлов в папке `tests/`.

### Основной корректный тест

```bash
make test
```

или:

```bash
make test_valid
```

Ожидаемый результат:

```text
Check: OK
```

### Проверка неправильного reset

```bash
make test_bad_reset
```

Ожидаемый результат: ошибка, так как `reset` должен быть равен `0` или `1`.

### Проверка неправильного valid

```bash
make test_bad_valid
```

Ожидаемый результат: ошибка, так как `valid` должен быть равен `0` или `1`.

### Проверка короткой строки

```bash
make test_bad_short
```

Ожидаемый результат: ошибка формата строки, так как параметров меньше пяти.

### Проверка длинной строки

```bash
make test_bad_long
```

Ожидаемый результат: ошибка формата строки, так как параметров больше пяти.

### Проверка нечислового значения

```bash
make test_nan
```

Ожидаемый результат: ошибка, так как один из параметров не является числом.

### Проверка пустого файла

```bash
make test_empty
```

Ожидаемый результат: ошибка, так как файл не содержит входных samples.

---

## Очистка

```bash
make clean
```

Удаляет собранный исполняемый файл:

```text
app
```

---

## Используемые возможности C++

В проекте используются:

- `std::vector`;
- `std::string`;
- `std::string_view`;
- `std::ifstream`;
- `std::istringstream`;
- исключения;
- структуры;
- классы;
- инкапсуляция;
- разбиение на `.h` и `.cpp`;
- сборка через `Makefile`.

---

## Учебная цель

Проект не является полноценной RTL-моделью и не использует Verilog/SystemVerilog.

Его цель — на C++ понять базовые идеи, важные при работе с RTL-моделями и retiming:

- регистр хранит состояние между тактами;
- pipeline добавляет задержку;
- `valid` нужен для отличия настоящих данных от пустых;
- `reset` может очистить pipeline и удалить отложенный результат;
- модели нельзя всегда сравнивать на одном и том же cycle;
- при проверке важно выводить не только `FAILED`, но и подробную диагностику ошибки.

---

## Текущий статус

Реализовано:

- комбинационная модель;
- pipeline-модель с задержкой 1 такт;
- обработка `valid`;
- обработка `reset`;
- flush-такт;
- сравнение моделей с учётом latency;
- reset-aware expected output;
- mismatch diagnostics;
- чтение input trace из файла;
- валидация входных данных;
- набор тестовых trace-файлов;
- сборка и запуск через `Makefile`.

Планируемые улучшения:

- автоматическая проверка ожидаемых сообщений об ошибках;
- поддержка комментариев в input trace;
- multi-stage pipeline;
- более аккуратное форматирование таблицы вывода;
- дополнительные сценарии для retiming.
