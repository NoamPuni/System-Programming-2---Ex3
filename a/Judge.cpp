#include "Judge.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>

// Constructor initializes the Judge with a name.
Judge::Judge(const std::string& name) : Player(name) {
    // Constructor initializes the Judge with a name.
}

// Allows the Judge to undo a bribe action.
bool Judge::undoBribe(Player& bribingPlayer) {
    // No specific logic needed here as coin reversal is handled by the Game/GUI logic.
    return true; // Judge can undo a bribe.
}

// Defines how the Judge reacts when sanctioned by another player.
void Judge::onSanctionedBy(Player& attacker, Game& game) {
    // If the target is a Judge, the player who sanctioned them loses 1 more coin.
    attacker.setCoins(-1);
}

// Returns the role of the player as "Judge".
std::string Judge::role() const {
    return "Judge"; // Returns the role of the player.
}