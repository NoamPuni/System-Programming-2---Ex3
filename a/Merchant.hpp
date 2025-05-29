#ifndef MERCHANT_HPP
#define MERCHANT_HPP

#include "Player.hpp"
#include <string>


class Merchant : public Player {
public:
    Merchant(const std::string& name);   

    // override
    void onBeginTurn() override;

    // override
    void onArrestedBy(Player& attacker, Game& game) override;
    std::string role() const override; // Returns the role of the player
};
#endif // MERCHANT_HPP