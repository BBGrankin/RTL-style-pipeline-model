CXX = g++
TARGET = app
SOURCES = main.cpp src/checker.cpp src/errors.cpp src/models.cpp src/samples.cpp
CXXFLAGS = -std=c++20 -Wall -Wextra -Iinclude

all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

test:
	./$(TARGET)

clean:
	rm -f ./$(TARGET)