# RTL-style Pipeline Model

Учебный C++ проект для моделирования RTL-style pipeline-поведения, проверки pipeline latency и демонстрации базовой идеи retiming.

Проект имитирует маленький verification flow: есть входной trace, C++ reference-модели, pipeline output, внешний actual output и checker, который сравнивает expected/actual значения по тактам.

---

## Основная идея

Базовое вычисление:

```text
y = (a + b) * c
```

В проекте есть несколько моделей:

- `CombinationModel` — комбинационная модель без задержки;
- `PipelinedModel` — pipeline-модель с задержкой `latency = 1`;
- `TwoStagePipelineModel` — двухстадийная pipeline-модель с задержкой `latency = 2`;
- `RetimedTwoStagePipelineModel` — альтернативная двухстадийная модель с другим расположением регистров, но той же observable latency.

Для internal comparison используется идея:

```text
comb_outputs[i] сравнивается с pipe_outputs[i + latency]
```

То есть:

```text
input на cycle N -> output pipeline на cycle N + latency
```

Для external actual-output comparison и retimed comparison сравнение идёт уже без сдвига:

```text
expected[i] сравнивается с actual[i]
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

## Что умеет программа

Программа поддерживает три основных режима проверки.

### 1. Internal model comparison

В этом режиме программа:

1. читает input trace;
2. прогоняет его через `CombinationModel`;
3. прогоняет его через выбранную pipeline-модель;
4. добавляет нужное количество flush-тактов;
5. сравнивает `comb[i]` с `pipe[i + latency]`;
6. учитывает `reset` на пути от input-cycle до output-cycle;
7. выводит таблицу и mismatch diagnostics.

### 2. External actual-output comparison

В этом режиме программа:

1. читает input trace;
2. строит expected pipeline output через C++ reference pipeline model;
3. читает внешний output-файл;
4. сравнивает `expected[i]` с `actual[i]` по каждому cycle;
5. проверяет размер expected/actual output;
6. выводит таблицу `expected/actual` и список mismatch'ей.

Это ближе к реальному verification flow:

```text
input trace
    -> C++ reference model
    -> expected output

actual_output.txt
    -> external output / RTL simulation output

checker
    -> expected vs actual
```

### 3. Retimed model comparison

В этом режиме программа сравнивает две latency=2 pipeline-модели:

```text
TwoStagePipelineModel
RetimedTwoStagePipelineModel
```

Смысл проверки:

```text
original_pipeline_output[i] == retimed_pipeline_output[i]
```

То есть две модели имеют разное внутреннее расположение регистров, но должны давать одинаковое внешнее поведение по cycle.

---

## Формат input trace

Файл входных данных содержит по одному такту на строку:

```text
reset valid a b c
```

Пример:

```text
1 0 0 0 0
0 1 2 3 4
0 1 1 5 2
0 0 9 9 9
0 1 3 4 5
1 0 0 0 0
0 1 9 2 3
0 0 0 0 0
```

Пустые строки допускаются и пропускаются.

---

## Формат actual output

Файл внешнего output содержит по одному output-cycle на строку:

```text
valid y
```

Пример:

```text
0 0
0 0
1 20
1 12
0 0
0 0
0 0
1 36
0 0
```

---

## Валидация файлов

Модуль `file_reader` проверяет input trace и actual output.

Для input trace проверяется:

- файл должен открываться;
- строка должна содержать ровно 5 параметров;
- все параметры должны быть корректными целыми числами;
- токены вроде `12abc`, `abc12`, `1.5`, `+`, `-` считаются ошибкой;
- числа должны помещаться в тип `int`;
- `reset` должен быть равен `0` или `1`;
- `valid` должен быть равен `0` или `1`;
- файл не должен быть пустым.

Для actual output проверяется:

- файл должен открываться;
- строка должна содержать ровно 2 параметра;
- оба параметра должны быть корректными целыми числами;
- `valid` должен быть равен `0` или `1`;
- файл не должен быть пустым.

Примеры ошибок:

```text
Wrong number of parameters in line N
Not a number in line N on position K
Could not convert string to int on line N on position K
Wrong valid / reset argument in line N
Wrong valid argument in output file on line N
Input file is empty
Output file is empty
Could not read file
```

---

## Mismatch diagnostics

### Internal checker

Internal checker выводит mismatch report с информацией:

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

Если reset произошёл на пути от input-cycle до output-cycle, expected output становится невалидным:

```text
expected valid = 0
expected y = 0
```

### External checker

External checker выводит таблицу:

```text
cycle | expected valid | expected y | actual valid | actual y | result
```

Где `result` равен:

```text
OK
FAIL
```

При ошибках дополнительно выводятся cycle, expected values и actual values.

### Retimed checker

Retimed checker выводит таблицу:

```text
cycle | original valid | original y | retimed valid | retimed y | result
```

Если `TwoStagePipelineModel` и `RetimedTwoStagePipelineModel` расходятся, checker выводит mismatch по соответствующему cycle.

---

## Структура проекта

```text
RTL-style-pipeline-model/
├── include/
│   ├── app_runner.h
│   ├── checker.h
│   ├── errors.h
│   ├── file_reader.h
│   ├── formatting.h
│   ├── models.h
│   └── samples.h
├── src/
│   ├── app_runner.cpp
│   ├── checker.cpp
│   ├── errors.cpp
│   ├── file_reader.cpp
│   ├── formatting.cpp
│   └── models.cpp
├── tests/
│   ├── valid_trace.txt
│   ├── latency_2.txt
│   ├── latency2_reset_i_plus_1.txt
│   ├── latency2_reset_i_plus_2.txt
│   ├── latency2_no_reset_path.txt
│   ├── actual_latency2_reset_i_plus_1.txt
│   ├── actual_latency2_reset_i_plus_2.txt
│   ├── actual_latency2_no_reset_path.txt
│   ├── actual_latency2_no_reset_path_bad.txt
│   ├── bad_reset.txt
│   ├── bad_valid.txt
│   ├── bad_format_short.txt
│   ├── bad_format_long.txt
│   ├── not_a_number.txt
│   └── empty.txt
├── input.txt
├── actual_output.txt
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

### Запуск с параметрами по умолчанию

```bash
make run
```

По умолчанию используются:

```text
INPUT_FILE=input.txt
OUTPUT_FILE=actual_output.txt
LATENCY=1
```

### Internal comparison

Запуск через Makefile:

```bash
make run INPUT_FILE=tests/valid_trace.txt LATENCY=1
```

```bash
make run INPUT_FILE=tests/latency_2.txt LATENCY=2
```

Прямой запуск:

```bash
./app tests/valid_trace.txt 1
```

```bash
./app tests/latency_2.txt 2
```

### External actual-output comparison

Формат прямого запуска:

```bash
./app input_trace.txt actual_output.txt latency
```

Пример:

```bash
./app tests/valid_trace.txt actual_output.txt 1
```

Для `latency = 2`:

```bash
./app tests/latency_2.txt tests/actual_latency2_no_reset_path.txt 2
```

---

## Makefile targets

### Основная сборка

```bash
make
```

### Запуск с параметрами по умолчанию

```bash
make run
```

### Internal tests

Основной корректный тест для `latency = 1`:

```bash
make test
```

или:

```bash
make test_valid
```

Корректный тест для `latency = 2`:

```bash
make test_latency_2
```

Reset-path сценарии для `latency = 2`:

```bash
make test_latency2_reset_i_plus_1
make test_latency2_reset_i_plus_2
make test_latency2_no_reset_path
```

### External actual-output tests

```bash
make test_input_output
```

```bash
make test_external_latency2_reset_i_plus_1
make test_external_latency2_reset_i_plus_2
make test_external_latency2_no_reset_path
make test_external_latency2_bad_actual
```

### Bad input tests

```bash
make test_bad_reset
make test_bad_valid
make test_bad_short
make test_bad_long
make test_nan
make test_empty
```

### Запуск всех тестовых сценариев

```bash
make test_all
```

### Очистка

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
- внешний actual output должен сравниваться с уже выровненным expected pipeline output;
- retiming может менять внутреннее расположение регистров, но observable output должен оставаться эквивалентным;
- при проверке важно выводить не только `FAILED`, но и подробную диагностику ошибки.

---

## Текущий статус

Реализовано:

- комбинационная модель;
- pipeline-модель с задержкой 1 такт;
- двухстадийная pipeline-модель с задержкой 2 такта;
- retimed двухстадийная pipeline-модель с задержкой 2 такта;
- обработка `valid`;
- обработка `reset`;
- flush-такты в зависимости от latency;
- internal comparison с учётом latency;
- reset-aware expected output;
- mismatch diagnostics;
- чтение input trace из файла;
- чтение external actual output из файла;
- external expected-vs-actual checker;
- comparison original-vs-retimed latency=2 models;
- строгая валидация входных данных;
- выбор latency через аргументы командной строки;
- набор тестовых trace-файлов;
- отдельные make-цели для тестов;
- аккуратное табличное форматирование вывода;
- сборка и запуск через `Makefile`.

Планируемые улучшения:

- автоматическая проверка ожидаемых сообщений об ошибках и exit codes;
- поддержка комментариев в input trace и actual output;
- обобщение pipeline-модели для произвольной latency;
- более сложные retiming-сценарии с несколькими операциями;
- возможное добавление output-файлов для отдельных latency/test cases.
