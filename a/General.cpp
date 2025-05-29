#include "General.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
General::General(const std::string& name) : Player(name) {
    // Constructor initializes the General with a name
}
void General::onArrestedBy(Player& attacker) {
    // General doesnt lose coins when arrested
    attacker.setCoins(0); // The attacker doesn't gain any coins
}


std::string General::role() const {
    return "General"; // Returns the role of the player
}

