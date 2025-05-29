CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system
INCLUDES = -I.

# Source files for GUI version
GUI_SRCS = main_gui.cpp Game.cpp Player.cpp Merchant.cpp Governor.cpp General.cpp Judge.cpp Spy.cpp Baron.cpp
GUI_OBJS = $(GUI_SRCS:.cpp=.o)
GUI_TARGET = coup_gui

# Source files for DEMO version
DEMO_SRCS = demo.cpp Game.cpp Player.cpp Merchant.cpp Governor.cpp General.cpp Judge.cpp Spy.cpp Baron.cpp
DEMO_OBJS = $(DEMO_SRCS:.cpp=.o)
DEMO_TARGET = coup_demo

# Default target - builds both
all: $(GUI_TARGET) $(DEMO_TARGET)

# GUI version target
gui: $(GUI_TARGET)

# DEMO version target
demo: $(DEMO_TARGET)

# Build GUI version
$(GUI_TARGET): $(GUI_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Build DEMO version (no SFML needed)
$(DEMO_TARGET): $(DEMO_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Run the demo
run-demo: $(DEMO_TARGET)
	./$(DEMO_TARGET)

# Run the GUI
run-gui: $(GUI_TARGET)
	./$(GUI_TARGET)

# Clean all object files and executables
clean:
	rm -f $(GUI_OBJS) $(DEMO_OBJS) $(GUI_TARGET) $(DEMO_TARGET)

# Clean only demo files
clean-demo:
	rm -f $(DEMO_OBJS) $(DEMO_TARGET)

# Clean only GUI files
clean-gui:
	rm -f $(GUI_OBJS) $(GUI_TARGET)

.PHONY: all gui demo run-demo run-gui clean clean-demo clean-gui

# לבנות ולהריץ את הדמו
#// make run-demo

# או בשלבים נפרדים:
#//  make demo
#// ./coup_demo

# לבנות את הגרסה הגרפית
#// make gui

#להריץ GUI
#// ./coup_gui

# לבנות הכל
#// make all

# לנקות
#// make clean