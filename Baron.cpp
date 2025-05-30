#include "Baron.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>

// Constructor initializes the Baron with a name.
Baron::Baron(const std::string& name) : Player(name) {
    // Constructor initializes the Baron with a name.
}

// Allows the Baron to invest, gaining coins at a cost.
void Baron::invest() {
    if (coins < 3) { // Checks cost.
        throw std::runtime_error(name + " doesn't have enough coins to invest (needs 3).");
    }
    setCoins(3); // Example: Baron can invest and gain 3 coins.
}

// Defines how the Baron reacts when sanctioned by another player.
void Baron::onSanctionedBy(Player& attacker, Game& game) {
    if (is_sanctioned) {
        throw std::runtime_error(name + " is already sanctioned.");
    }
    is_sanctioned = true; // Mark the Baron as sanctioned.
    attacker.setCoins(-1); // The player who sanctioned the Baron loses 1 coin.
    setCoins(1); // When sanctioned, the Baron gains 1 coin.
}

// Returns the role of the player as "Baron".
std::string Baron::role() const {
    return "Baron"; // Returns the role of the player.
}