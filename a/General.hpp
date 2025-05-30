#ifndef GENERAL_HPP
#define GENERAL_HPP

#include "Player.hpp"
#include <string>

class General : public Player {
    public:
        General(const std::string& name); // Constructor for the General class.

        void blockCoup(Player& targetPlayerOfCoup, Player& originalAttacker); // Allows the General to block a coup.

        bool canBlockCoup() const override { return true; } // General can block coups.
        std::string role() const override; // Returns the role of the player.
        void onArrestedBy(Player& attacker, Game& game) override; // Defines how the General reacts when arrested.
    };

#endif // GENERAL_HPP