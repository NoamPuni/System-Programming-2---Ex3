#include "Governor.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <string>
#include <iostream>

// Constructor initializes the Governor with a name.
Governor::Governor(const std::string& name) : Player(name) {
    // Constructor initializes the Governor with a name.
}

// Returns the amount of coins the tax action would yield for the Governor.
int Governor::tax(Game& game) { 
    if (is_sanctioned) {
        throw std::runtime_error(name + " is sanctioned and can't tax.");
    }
    return 3; // Governor taxes 3 coins.
}

// Returns the role of the player as "Governor".
std::string Governor::role() const {
    return "Governor"; // Returns the role of the player.
}

// Indicates that the Governor can block tax actions.
bool Governor::canBlockTax() const {
    return true; // Governor can block tax actions.
}