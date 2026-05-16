# RTL-style Pipeline Model

Учебный C++ проект для моделирования RTL-style pipeline-поведения и проверки результатов с учётом pipeline latency.

Проект имитирует базовую verification-задачу: есть комбинационная модель без задержки и pipeline-модели с задержкой. Программа читает входной trace из файла, валидирует данные, прогоняет их через модели, сравнивает expected/actual outputs с учётом `latency`, `valid`, `reset` и выводит mismatch diagnostics при расхождении.

---

## Основная идея

Базовое вычисление:

```text
y = (a + b) * c
```

В проекте есть несколько моделей:

- `CombinationModel` — комбинационная модель без задержки;
- `PipelinedModel` — pipeline-модель с задержкой `latency = 1`;
- `TwoStagePipelineModel` — двухстадийная pipeline-модель с задержкой `latency = 2`.

Для `latency = 1` сравнение выполняется так:

```text
comb_outputs[i] сравнивается с pipe_outputs[i + 1]
```

Для `latency = 2`:

```text
comb_outputs[i] сравнивается с pipe_outputs[i + 2]
```

Общий принцип:

```text
input на cycle N -> output pipeline на cycle N + latency
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

Если `valid = 0`, это не reset. Такой такт добавляет bubble в pipeline, но не очищает уже находящиеся внутри данные.

---

## Что делает программа

Программа:

1. читает входной trace из файла;
2. валидирует формат входных данных;
3. выбирает режим проверки по latency;
4. прогоняет данные через `CombinationModel`;
5. прогоняет данные через выбранную pipeline-модель;
6. добавляет нужное количество flush-тактов;
7. печатает таблицу по всем cycle;
8. сравнивает выходы моделей с учётом latency;
9. учитывает `reset` на пути от input-cycle до output-cycle;
10. выводит `Check: OK` или `Check: FAILED`;
11. при ошибке выводит mismatch report.

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
- все параметры должны быть корректными целыми числами;
- токены вроде `12abc`, `abc12`, `1.5`, `+`, `-` считаются ошибкой;
- числа должны помещаться в тип `int`;
- `reset` должен быть равен `0` или `1`;
- `valid` должен быть равен `0` или `1`;
- файл не должен быть пустым.

Примеры ошибок:

```text
Wrong number of parameters in line N
Not a number in line N on position K
Could not convert string to int on line N on position K
Wrong valid / reset argument in line N
File is empty
Could not read file
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
- повлиял ли `reset` на путь прохождения данных по pipeline;
- какая latency использовалась при сравнении.

Если reset произошёл на пути от input-cycle до output-cycle, ожидаемый pipeline output становится невалидным:

```text
expected valid = 0
expected y = 0
```

---

## Структура проекта

```text
RTL-style-pipeline-model/
├── include/
│   ├── app_runner.h
│   ├── checker.h
│   ├── errors.h
│   ├── formatting.h
│   ├── models.h
│   ├── samples.h
│   └── trace_reader.h
├── src/
│   ├── app_runner.cpp
│   ├── checker.cpp
│   ├── errors.cpp
│   ├── models.cpp
│   └── trace_reader.cpp
├── tests/
│   ├── valid_trace.txt
│   ├── latency_2.txt
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

По умолчанию используется:

```text
FILE=input.txt
LATENCY=1
```

### Запуск с произвольным trace-файлом и latency

```bash
make run FILE=tests/valid_trace.txt LATENCY=1
```

```bash
make run FILE=tests/latency_2.txt LATENCY=2
```

Также можно запускать программу напрямую:

```bash
./app tests/valid_trace.txt 1
```

```bash
./app tests/latency_2.txt 2
```

Если файл не передан, используется `input.txt`.

---

## Тестовые сценарии

В проекте есть набор входных trace-файлов в папке `tests/`.

### Основной корректный тест для latency = 1

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

### Корректный тест для latency = 2

```bash
make test_latency_2
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

### Запуск всех тестовых сценариев

```bash
make test_all
```

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
- `std::format`;
- `std::stoi`;
- исключения;
- структуры;
- классы;
- инкапсуляция;
- разбиение на `.h` и `.cpp`;
- header-only файл для простых структур;
- отдельный модуль запуска `app_runner`;
- сборка через `Makefile`.

---

## Учебная цель

Проект не является полноценной RTL-моделью и не использует Verilog/SystemVerilog.

Его цель — на C++ понять базовые идеи, важные при работе с RTL-моделями, retiming и verification:

- регистр хранит состояние между тактами;
- pipeline добавляет задержку;
- `valid` нужен для отличия настоящих данных от пустых;
- `valid = 0` создаёт bubble, но не очищает весь pipeline;
- `reset` может очистить pipeline и удалить отложенный результат;
- модели нельзя всегда сравнивать на одном и том же cycle;
- latency должна учитываться при сравнении expected/actual;
- reset должен учитываться на пути от input-cycle до output-cycle;
- при проверке важно выводить не только `FAILED`, но и подробную диагностику ошибки.

---

## Текущий статус

Реализовано:

- комбинационная модель;
- pipeline-модель с задержкой 1 такт;
- двухстадийная pipeline-модель с задержкой 2 такта;
- обработка `valid`;
- обработка `reset`;
- flush-такты в зависимости от latency;
- сравнение моделей с учётом latency;
- reset-aware expected output;
- mismatch diagnostics;
- чтение input trace из файла;
- строгая валидация входных данных;
- выбор latency через аргументы командной строки;
- набор тестовых trace-файлов;
- отдельные make-цели для тестов;
- аккуратное табличное форматирование вывода;
- сборка и запуск через `Makefile`.

Планируемые улучшения:

- автоматическая проверка ожидаемых сообщений об ошибках и exit codes;
- поддержка комментариев в input trace;
- обобщение pipeline-модели для произвольной latency;
- добавление внешнего `actual_output.txt` и сравнение reference output с external output;
- дополнительные сценарии для retiming.
