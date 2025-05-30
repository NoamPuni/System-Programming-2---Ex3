#ifndef GOVERNOR_HPP
#define GOVERNOR_HPP
#include "Player.hpp"
#include <string>
#include <stdexcept>
#include <iostream> 
#include <memory>
class Governor : public Player
{

public:
    Governor(const std::string& name); // Constructor for the Governor class.
    ~Governor() = default; // Default destructor for Governor.

    int tax(Game& game) override; // Governor can tax 3 coins, returns amount.
    std::string role() const override; // Returns the role of the player.
    bool canBlockTax() const override; // Governor can block tax actions.
};

#endif