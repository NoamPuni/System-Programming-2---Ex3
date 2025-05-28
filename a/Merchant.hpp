#ifndef MERCHANT_HPP
#define MERCHANT_HPP

#include "Player.hpp"
#include <string>


class Merchant : public Player {
public:
    Merchant(const std::string& name);
    ~Merchant() override = default;

    // override
    void onBeginTurn() override;

    // override
    void onArrestedBy(Player& attacker) override;
    std::string role() const override; // Returns the role of the player
};
#endif // MERCHANT_HPP