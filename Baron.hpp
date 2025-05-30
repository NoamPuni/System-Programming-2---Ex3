#ifndef BARON_HPP
#define BARON_HPP

#include "Player.hpp"
#include <string>


class Baron : public Player {
public:
    Baron(const std::string& name); // Constructor for the Baron class.

    void invest(); // Allows the Baron to invest.
    std::string role() const override; // Returns the role of the player.

    // Override methods
    void onSanctionedBy(Player& attacker, Game& game) override; // Defines how the Baron reacts when sanctioned.
};

#endif // BARON_HPP