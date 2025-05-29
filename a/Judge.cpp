#include "Judge.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
Judge::Judge(const std::string& name) : Player(name) {
    // Constructor initializes the Judge with a name
}
bool Judge::undoBribe(Player& bribingPlayer) {
    return true; // Judge can undo a bribe
}
void Judge::onSanctionedBy(Player& attacker, Game& game) {
    // If the target is a Judge, the player who sanctioned him loses 1 more coin
        attacker.setCoins(-1);
    }
std::string Judge::role() const {
    return "Judge"; // Returns the role of the player
}
