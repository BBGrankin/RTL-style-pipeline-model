CXX = g++
TARGET = app
SOURCES = main.cpp src/checker.cpp src/errors.cpp src/models.cpp src/samples.cpp src/trace_reader.cpp
CXXFLAGS = -std=c++20 -Wall -Wextra -Iinclude

LONG = tests/bad_format_long.txt
SHORT = tests/bad_format_short.txt
BAD_RESET = tests/bad_reset.txt
BAD_VALID = tests/bad_valid.txt
EMPTY = tests/empty.txt
NAN = tests/not_a_number.txt
VALID = tests/valid_trace.txt

FILE = input.txt

.PHONY: all test test_valid test_bad_long test_bad_short test_bad_reset test_bad_valid test_empty test_nan clean run

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: all
	./$(TARGET) $(FILE)

test_default: all
	./$(TARGET)

test: all
	./$(TARGET) $(VALID)

test_bad_long: all
	./$(TARGET) $(LONG)

test_bad_short: all
	./$(TARGET) $(SHORT)

test_bad_reset: all
	./$(TARGET) $(BAD_RESET)

test_bad_valid: all
	./$(TARGET) $(BAD_VALID)

test_empty: all
	./$(TARGET) $(EMPTY)

test_nan: all
	./$(TARGET) $(NAN)

clean:
	rm -f ./$(TARGET)