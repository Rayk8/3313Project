CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -Iinclude

SRC = src/main.cpp src/server.cpp src/library.cpp src/auth.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = library_server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
