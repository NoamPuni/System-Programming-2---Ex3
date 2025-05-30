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

# Source files for TEST version
TEST_SRCS = TestGame.cpp Game.cpp Player.cpp Merchant.cpp Governor.cpp General.cpp Judge.cpp Spy.cpp Baron.cpp
TEST_OBJS = $(TEST_SRCS:.cpp=.o)
TEST_TARGET = coup_test

# Default target - builds both
all: $(GUI_TARGET) $(DEMO_TARGET) $(TEST_TARGET)

# GUI version target
gui: $(GUI_TARGET)

# DEMO version target
demo: $(DEMO_TARGET)

# TEST version target
test: $(TEST_TARGET)

# Build GUI version
$(GUI_TARGET): $(GUI_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Build DEMO version (no SFML needed)
$(DEMO_TARGET): $(DEMO_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build TEST version (no SFML needed, uses doctest)
$(TEST_TARGET): $(TEST_OBJS)
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

# Run the tests
run-test: $(TEST_TARGET)
	./$(TEST_TARGET)

# Run valgrind for demo
valgrind-demo: $(DEMO_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(DEMO_TARGET)

# Run valgrind for tests
valgrind-test: $(TEST_TARGET)
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(TEST_TARGET)

# Clean all object files and executables
clean:
	rm -f $(GUI_OBJS) $(DEMO_OBJS) $(TEST_OBJS) $(GUI_TARGET) $(DEMO_TARGET) $(TEST_TARGET)

# Clean only demo files
clean-demo:
	rm -f $(DEMO_OBJS) $(DEMO_TARGET)

# Clean only GUI files
clean-gui:
	rm -f $(GUI_OBJS) $(GUI_TARGET)

# Clean only test files
clean-test:
	rm -f $(TEST_OBJS) $(TEST_TARGET)

.PHONY: all gui demo test run-demo run-gui run-test valgrind-demo valgrind-test clean clean-demo clean-gui clean-test

	