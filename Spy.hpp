#ifndef SPY_HPP
#define SPY_HPP

#include "Player.hpp"
#include <string>

class Spy : public Player {
public:
    Spy(const std::string& name); // Constructor for the Spy class.

    void revealCoins(Player& targetPlayer) const; // Reveals the number of coins of a target player.
    void preventArrest(Player& targetPlayer); // Prevents a target player from using arrest on their next turn.
    bool canPreventArrest() const override {return true;} // Indicates that the Spy can always prevent an arrest.
    std::string role() const override; // Returns the role of the player.
};

#endif // SPY_HPP