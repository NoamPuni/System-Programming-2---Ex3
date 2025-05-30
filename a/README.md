Coup Game Project
This project implements a simplified version of "Coup," a tabletop card game for 2-6 players, focusing on core mechanics and role-based special abilities. The goal is to be the last player remaining in the game.

Game Overview
In Coup, players take on hidden roles, each with unique abilities. The game revolves around managing your coin count, performing actions, and strategically eliminating other players. The last player with a role remaining wins the game.

Core Mechanics
Each player has a name, a role, and a certain number of coins. On their turn, a player can perform one of the following general actions, regardless of their role:

Gather: Gain 1 coin from the treasury. This action has no cost and can be blocked by a Sanction.
Tax: Gain 2 coins from the treasury. No cost, but certain roles or actions can block it.
Bribe: Pay 4 coins to gain an additional action during your turn.
Arrest: Choose another player and take 1 coin from them. Cannot be used on the same player twice in a row.
Sanction: Choose another player and prevent them from using economic actions (Gather, Tax) until their next turn. Costs 3 coins.
Coup: Choose another player and eliminate them from the game. Costs 7 coins and can only be blocked under specific conditions.
All actions update the player's coin count accordingly. Attempting an illegal action will throw an appropriate exception.

Roles and Special Abilities
The game features several distinct roles, each with unique powers:

Governor:
Takes 3 coins from the treasury (instead of 2) when performing a Tax action.
Can cancel other players' Tax actions.
Spy:
Can reveal another player's coin count (does not cost a turn or coins).
Can prevent another player from using an Arrest action on their next turn (does not cost a turn or coins).
Baron:
Can "invest" 3 coins to gain 6 coins in return.
If targeted by a Sanction, the Baron receives 1 coin as compensation.
General:
Can pay 5 coins to block a Coup action against another player (or themselves). If blocked, the player who initiated the Coup still loses their 7 coins.
If targeted by an Arrest, the General does not lose any coins (the attacker gains nothing).
Judge:
Can undo another player's Bribe action, causing the bribing player to lose their 4 coins.
If targeted by a Sanction, the player who initiated the Sanction must pay an additional coin to the treasury.
Merchant:
If the Merchant starts their turn with 3 or more coins, they gain 1 additional coin for free.
If targeted by an Arrest, the Merchant pays 2 coins to the treasury (instead of losing 1 to the attacker).
Game Rules & Notes
Multiple players can have the same role, and no special interactions occur in such cases.
If a player holds 10 or more coins at the beginning of their turn, they must perform a Coup action that turn.
Blocking actions (e.g., a Judge blocking a bribe, a General blocking a coup) do not consume a turn and are performed "in real-time" conceptually. In this implementation, a blocked action remains blockable until the original action performer's next turn.
Project Structure
The project is organized into several C++ files, each representing a core game component or a specific player role:

Game.hpp/Game.cpp: Manages the overall game state, player turns, and game progression.
Player.hpp/Player.cpp: Base class for all players, defining common attributes and actions.
Baron.hpp/Baron.cpp: Implements the Baron role and its unique abilities.
Governor.hpp/Governor.cpp: Implements the Governor role and its unique abilities.
General.hpp/General.cpp: Implements the General role and its unique abilities.
Judge.hpp/Judge.cpp: Implements the Judge role and its unique abilities.
Merchant.hpp/Merchant.cpp: Implements the Merchant role and its unique abilities.
Spy.hpp/Spy.cpp: Implements the Spy role and its unique abilities.
main_gui.cpp: Contains the graphical user interface (GUI) implementation using SFML.
demo.cpp: Provides a command-line demonstration of game mechanics.
TestGame.cpp: Contains unit tests for the game logic, using doctest.
Build Instructions
The project uses a Makefile for compilation.

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system (for GUI builds)
INCLUDES = -I.
Available Make Targets:

all: Builds all targets (coup_gui, coup_demo, coup_test).
gui: Builds the graphical user interface executable (coup_gui).
demo: Builds the command-line demonstration executable (coup_demo).
test: Builds the unit test executable (coup_test).
clean: Removes all compiled object files and executables.
clean-demo: Removes demo-specific object files and executable.
clean-gui: Removes GUI-specific object files and executable.
clean-test: Removes test-specific object files and executable.
How to Run
Run Demo: make run-demo
Run GUI: make run-gui
Run Tests: make run-test
Debugging & Memory Checks
Valgrind Demo: make valgrind-demo (runs Valgrind on the demo executable for memory leak detection).
Valgrind Tests: make valgrind-test (runs Valgrind on the test executable).
Feel free to explore the code, run the demo, and test the game mechanics!