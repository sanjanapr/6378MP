CXX = g++
CXXFLAGS = -Wall -std=c++17 -pthread
LDFLAGS = -pthread

SOURCES = main.cpp config.cpp graph.cpp connection.cpp network.cpp socket_util.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = node

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
