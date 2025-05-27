#pragma once
#include "General.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
General::General(const std::string& name) : Player(name) {
    // Constructor initializes the General with a name
}
General::~General() {
    // Destructor for General
}
void General::onArrestedBy(Player& attacker) {
    // General's specific behavior when arrested by another player
    setCoins(0); // General doesn't lose coins when arrested
    
}

