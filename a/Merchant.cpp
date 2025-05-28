#pragma once
#include "Merchant.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
Merchant::Merchant(const std::string& name) : Player(name) {
    // Constructor initializes the Merchant with a name
}
Merchant::~Merchant() {
    // Destructor for Merchant
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
void Merchant::onArrestedBy(Player& attacker) {
    attacker.setCoins(-1); // The attacker loses the 1 gotten coin when arresting the Merchant
    setCoins(-1); // The Merchant loses another 1 coin when arrested
}
std::string Merchant::role() const {
    return "Merchant"; // Returns the role of the player
}
