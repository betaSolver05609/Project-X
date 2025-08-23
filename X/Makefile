# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I. 
LDFLAGS = -ljsoncpp

# Find all .cpp files recursively
SRC = $(shell find . -name "*.cpp")

OUT = db_start

# Default target
all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

# Run the program
run: $(OUT)
	./$(OUT)

# Clean compiled files
clean:
	rm -f $(OUT)
