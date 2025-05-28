CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
INCLUDES = -I.
SRCS = main_gui.cpp Game.cpp Player.cpp Merchant.cpp Governor.cpp General.cpp Judge.cpp Spy.cpp Baron.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = coup_gui

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
