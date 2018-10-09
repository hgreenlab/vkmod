CXX = g++
LDLIBS = -lvulkan -lglfw
SRC = main.cpp
LDFLAGS = -std=c++11 -g

all:
	$(CXX) $(LDFLAGS) $(SRC) -o main $(LDLIBS)
