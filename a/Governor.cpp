#include "Governor.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
Governor::Governor(const std::string& name) : Player(name) {
    // Constructor initializes the Governor with a name
}
void Governor::tax(Game& game) {
        if (is_sanctioned) {
            throw std::runtime_error(name + " is sanctioned and can't tax.");
        }
        else {
            setCoins(3); // Governor can tax 3 coins
        }
}
void Governor::blockTax(Game& game) {
    if (!isAlive()) {
        throw std::runtime_error(name + " is not alive and cannot block tax.");
    }
    game.tryBlockTax(this);
}
std::string Governor::role() const {
    return "Governor"; // Returns the role of the player
}

