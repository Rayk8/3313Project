CXX = g++
CXXFLAGS = -std=c++11 -Wall -pthread

SRC = src/main.cpp src/server.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = library_server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
