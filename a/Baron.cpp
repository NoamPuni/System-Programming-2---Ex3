#include "Baron.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
Baron::Baron(const std::string& name) : Player(name) {
    // Constructor initializes the Baron with a name
}
void Baron::invest() {
        setCoins(3); // Example: Baron can invest and gain 3 coins
    }
void Baron::onSanctionedBy(Player& attacker) {
    if (is_sanctioned) {
        throw std::runtime_error(name + " is already sanctioned.");
    }
    is_sanctioned = true; // Mark the Baron as sanctioned
    attacker.setCoins(-1);// the player who sanctioned the Baron loses more 1 coin
    setCoins(1); // when sanctioned, the Baron gets more 1 coin
}
std::string Baron::role() const {
    return "Baron"; // Returns the role of the player
}



