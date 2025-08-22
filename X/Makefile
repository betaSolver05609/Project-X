# Compiler
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I. 
LDFLAGS = -ljsoncpp

# Project files
SRC = db-start.cpp
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
