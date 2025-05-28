#include "Spy.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
Spy::Spy(const std::string& name) : Player(name) {
    // Spy-specific initialization can go here if needed
}
void Spy::revealCoins(Player& targetPlayer) const {
    if (!targetPlayer.isAlive()) {
        throw std::runtime_error("Cannot reveal coins of a non-active player.");
    }
    std::cout << targetPlayer.getName() << "'s coins: " << targetPlayer.getCoins() << std::endl;
}
void Spy::preventArrest(Player& targetPlayer) {
    if (!targetPlayer.isAlive()) {
        throw std::runtime_error("Cannot prevent arrest for a non-active player.");
    }
    if (targetPlayer.isPreventedFromArresting()) {
        throw std::runtime_error(targetPlayer.getName() + " is already prevented from arresting.");
    }
    targetPlayer.gotPreventedFromArresting(); // Mark the target player as prevented from arresting
}
std::string Spy::role() const {
    return "Spy"; // Returns the role of the player
}
