#include "Merchant.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
Merchant::Merchant(const std::string& name) : Player(name) {
    // Constructor initializes the Merchant with a name
}
void Merchant::onBeginTurn() {
    if (coins >= 3) {
        setCoins(1); // Merchant gathers 1 coin at the beginning of their turn
    }
    if (coins >= 10)
    {
        std::cout << name << " has too many coins and must coup or lose coins."<< std::endl;
    }
}
void Merchant::onArrestedBy(Player& attacker, Game& game) {
    if (coins < 2) {
        throw std::runtime_error("Merchant doesn't have enough coins to be arrested.");
    }
    this->setCoins(-2);             // Merchant pays two to the treasury
    attacker.setCoins(0);          // The attacker doesn't gain anything
}

std::string Merchant::role() const {
    return "Merchant"; // Returns the role of the player
}
