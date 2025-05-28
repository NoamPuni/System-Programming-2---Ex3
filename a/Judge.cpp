#pragma once
#include "Judge.hpp"
#include "Player.hpp"
#include <stdexcept>
#include <string>
#include <iostream>
Judge::Judge(const std::string& name) : Player(name) {
    // Constructor initializes the Judge with a name
}
Judge::~Judge() = default;
void undoBribe(Player& bribingPlayer){}
void onSanctionedBy(Player& attacker) {
    // If the target is a Judge, the player who sanctioned him loses 1 more coin
        attacker.setCoins(-1);
    }
