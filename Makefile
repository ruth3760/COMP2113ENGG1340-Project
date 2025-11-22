CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I.

SRCS = main.cpp player.cpp relationships.cpp events.cpp activities.cpp savegame.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = summer_maxxing

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f savegame.txt