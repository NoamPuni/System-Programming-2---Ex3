#ifndef MERCHANT_HPP
#define MERCHANT_HPP

#include "Player.hpp"
#include <string>


class Merchant : public Player {
public:
    Merchant(const std::string& name); // Constructor for the Merchant class.

    void onBeginTurn() override; // Defines actions taken at the beginning of the Merchant's turn.

    void onArrestedBy(Player& attacker, Game& game) override; // Defines how the Merchant reacts when arrested.
    std::string role() const override; // Returns the role of the player.
};
#endif // MERCHANT_HPP