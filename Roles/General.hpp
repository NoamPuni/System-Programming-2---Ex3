#ifndef GENERAL_HPP
#define GENERAL_HPP

#include "Player.hpp"
#include <string>

class General : public Player {
    public:
        General(const std::string& name);
        ~General() override = default;

        void blockCoup(Player& targetPlayerOfCoup, Player& originalAttacker);//(-5)

        bool canBlockCoup() const override { return true; } // General can block coups
        //override methods
        std::string role() const override { return "General"; } // Returns the role of the player
        void onArrestedBy(Player& attacker) override;
    };

#endif // GENERAL_HPP