#include "Spy.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>

// Constructor initializes the Spy with a name.
Spy::Spy(const std::string& name) : Player(name) {
    // Spy-specific initialization can go here if needed.
}

// Allows the Spy to reveal the coin count of a target player.
void Spy::revealCoins(Player& targetPlayer) const {
    if (!targetPlayer.isAlive()) {
        throw std::runtime_error("Cannot reveal coins of a non-active player.");
    }
    std::cout << targetPlayer.getName() << "'s coins: " << targetPlayer.getCoins() << std::endl;
}

// Allows the Spy to prevent a target player from performing an arrest.
void Spy::preventArrest(Player& targetPlayer) {
    if (!targetPlayer.isAlive()) {
        throw std::runtime_error("Cannot prevent arrest for a non-active player.");
    }
    if (targetPlayer.isPreventedFromArresting()) {
        throw std::runtime_error(targetPlayer.getName() + " is already prevented from arresting.");
    }
    targetPlayer.gotPreventedFromArresting(); // Mark the target player as prevented from arresting.
}

// Returns the role of the player as "Spy".
std::string Spy::role() const {
    return "Spy"; // Returns the role of the player.
}