CXX = g++
TARGET = app
SOURCES = main.cpp src/checker.cpp\
				   src/errors.cpp\
				   src/models.cpp\
				   src/trace_reader.cpp\
				   src/app_runner.cpp\
				   src/formatting.cpp\
				   src/output_reader.cpp

CXXFLAGS = -std=c++20 -Wall -Wextra -Iinclude

LONG = tests/bad_format_long.txt
SHORT = tests/bad_format_short.txt
BAD_RESET = tests/bad_reset.txt
BAD_VALID = tests/bad_valid.txt
EMPTY = tests/empty.txt
NAN = tests/not_a_number.txt
VALID = tests/valid_trace.txt
LATENCY_2 = tests/latency_2.txt
OUTPUT = actual_output.txt

INPUT_FILE ?= input.txt
LATENCY ?= 1
OUTPUT_FILE ?= actual_output.txt

.PHONY: all\
		test\
		test_all\
		test_valid\
		test_bad_long\
		test_bad_short\
		test_bad_reset\
		test_bad_valid\
		test_empty\
		test_nan\
		clean\
		run\
		test_default\
		test_latency_2

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) $(INPUT_FILE) $(OUTPUT_FILE) $(LATENCY)

test_default: $(TARGET)
	./$(TARGET)

test: $(TARGET)
	./$(TARGET) $(VALID) 1

test_valid: $(TARGET)
	./$(TARGET) $(VALID) 1 

test_bad_long: $(TARGET)
	./$(TARGET) $(LONG) 1

test_bad_short: $(TARGET)
	./$(TARGET) $(SHORT) 1

test_bad_reset: $(TARGET)
	./$(TARGET) $(BAD_RESET) 1

test_bad_valid: $(TARGET)
	./$(TARGET) $(BAD_VALID) 1

test_empty: $(TARGET)
	./$(TARGET) $(EMPTY) 1

test_nan: $(TARGET)
	./$(TARGET) $(NAN) 1

test_latency_2: $(TARGET)
	./$(TARGET) $(LATENCY_2) 2 

test_all: $(TARGET)
	./$(TARGET) $(VALID) 1 
	./$(TARGET) $(LATENCY_2) 2
	./$(TARGET) $(BAD_RESET) 1 
	./$(TARGET) $(BAD_VALID) 1 
	./$(TARGET) $(SHORT) 1 
	./$(TARGET) $(LONG) 1 
	./$(TARGET) $(NAN) 1 
	./$(TARGET) $(EMPTY) 1 

clean:
	rm -f ./$(TARGET)