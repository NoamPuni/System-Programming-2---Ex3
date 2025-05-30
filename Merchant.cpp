#include "Merchant.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>

// Constructor initializes the Merchant with a name.
Merchant::Merchant(const std::string& name) : Player(name) {
    // Constructor initializes the Merchant with a name.
}

// Defines actions taken at the beginning of the Merchant's turn.
void Merchant::onBeginTurn() {
    if (coins >= 3) {
        setCoins(1); // Merchant gathers 1 coin at the beginning of their turn if they have 3 or more.
    }
    if (coins >= 10) {
        std::cout << name << " has too many coins and must coup or lose coins."<< std::endl;
    }
}

// Defines how the Merchant reacts when arrested by another player.
void Merchant::onArrestedBy(Player& attacker, Game& game) {
    if (coins < 2) {
        throw std::runtime_error("Merchant doesn't have enough coins to be arrested.");
    }
    this->setCoins(-2);            // Merchant pays two coins to the treasury.
    attacker.setCoins(0);          // The attacker doesn't gain anything.
}

// Returns the role of the player as "Merchant".
std::string Merchant::role() const {
    return "Merchant"; // Returns the role of the player.
}