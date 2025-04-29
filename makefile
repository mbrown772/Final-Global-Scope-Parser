#Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall

# Executable name
TARGET = compile

# Source files
SRC = parser.cpp scanner.cpp language.cpp main.cpp tree.cpp statSem.cpp compiler.cpp

# Object files (each .cpp file becomes a .o file)
OBJ = $(SRC:.cpp=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)
	rm -f $(OBJ)  # Remove .o files after building the executable

# Compile each source file into an object file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Phony targets
.PHONY: all clean

