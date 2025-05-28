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
    Governor(const std::string& name);
    ~Governor() = default;

    void tax(Game& game) override; // Governor can tax 3 coins
    void blockTax(Game& game); // Governor can block tax actions
    std::string role() const override; // Returns the role of the player
    bool canBlockTax() const override; // Governor can block tax actions
};

#endif
