#include "General.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>

// Constructor initializes the General with a name.
General::General(const std::string& name) : Player(name) {
    // Constructor initializes the General with a name.
}

// Allows the General to block a coup.
void General::blockCoup(Player& targetPlayerOfCoup, Player& originalAttacker) {
    // This method is called conceptually. Actual coin deduction and coup reversal are handled by the Game/GUI logic.
    return; // No specific logic needed here, as coin reversal and elimination restore are handled externally.
}

// Defines how the General reacts when arrested by another player.
void General::onArrestedBy(Player& attacker, Game& game) {
    // General does not lose coins when arrested.
    attacker.setCoins(0); // The attacker doesn't gain any coins from arresting the General.
}

// Returns the role of the player as "General".
std::string General::role() const {
    return "General"; // Returns the role of the player.
}