#pragma once
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
    ~Governor();

    void tax() override; // Governor can tax 3 coins
    void blockTax(); // Governor can block tax actions
    std::string role() const override { return "Governor"; } // Returns the role of the player
    bool canBlockTax() const override { return true; } // Governor can block tax actions
};

#endif
