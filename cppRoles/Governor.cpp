#pragma once
#include "Governor.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
Governor::Governor(const std::string& name) : Player(name) {
    // Constructor initializes the Governor with a name
}
Governor::~Governor() {
    // Destructor for Governor, currently does nothing special
}
void Governor::tax() {
        if (is_sanctioned) {
            throw std::runtime_error(name + " is sanctioned and can't tax.");
        }
        else {
            setCoins(3); // Governor can tax 3 coins
        }
    }