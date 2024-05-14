# Compiler
CC := g++

# Compiler flags
CFLAGS := -Wall -Wextra -std=c++17

LDFLAGS = -lfcgi

# Source files

# Header files
HDRS := header.hpp

# Object files directory
OBJDIR := obj

# Object files

# Target executable
TARGET := webserver

# Default target
all: $(TARGET)

# Create the object files directory if it doesn't exist
$(shell mkdir -p $(OBJDIR))

# Compile source files into object files
# Source files
SRCS1 := $(wildcard sockets/*.cpp)
SRCS2 := $(wildcard server/*.cpp)

# Object files
OBJS1 := $(addprefix $(OBJDIR)/,$(notdir $(SRCS1:.cpp=.o)))
OBJS2 := $(addprefix $(OBJDIR)/,$(notdir $(SRCS2:.cpp=.o)))
OBJS := $(OBJS1) $(OBJS2)

# Compile source files into object files
$(OBJDIR)/%.o: sockets/%.cpp $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: server/%.cpp $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) 

# Phony targets
.PHONY: all clean

# Clean up object files and the target executable
clean:
	rm -rf $(OBJDIR) $(TARGET)
