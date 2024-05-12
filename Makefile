# Compiler
CC := g++

# Compiler flags
CFLAGS := -Wall -Wextra -std=c++17

# Source files
SRCS := socket.cpp bind.cpp connect.cpp

# Header files
HDRS := socket.hpp bind.hpp connect.hpp

# Object files
OBJS := $(SRCS:.cpp=.o)

# Target executable
TARGET := webserver

# Default target
all: $(TARGET)

# Compile source files into object files
%.o: %.cpp $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Clean up object files and the target executable
clean:
	rm -f $(OBJS) $(TARGET)