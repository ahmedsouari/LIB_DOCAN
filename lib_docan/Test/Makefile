# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Source files
SRCS = ../Test/main.cpp \
       ../src/cantp.cpp \
       ../src/idocan.cpp \
       ../src/linklayer.cpp

# Include directory
INCLUDES = -I../include

# Output binary
TARGET = test_program

# Default target
all: $(TARGET)

# Build target
$(TARGET):
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(TARGET) $(SRCS) -lpthread

# Clean build files
clean:
	rm -rf $(TARGET)

# Phony targets
.PHONY: all clean $(TARGET)
