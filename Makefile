CXX = g++
TARGET = app
SOURCES = main.cpp
CXXFLAGS = -std=c++20 -Wall -Wextra -Iinclude

all:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

test:
	./$(TARGET)

clean:
	rm -f ./$(TARGET)