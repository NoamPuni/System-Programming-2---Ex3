#ifndef SPY_HPP
#define SPY_HPP

#include "Player.hpp"
#include <string>

    class Spy : public Player {
    public:
        Spy(const std::string& name);
        ~Spy() override = default;

        // special abilities of Spy - doesnt count as a turn
        void revealCoins(Player& targetPlayer) const; // reveals the number of coins of targetPlayer
        void preventArrest(Player& targetPlayer);    // prevents targetPlayer from using arrest on their next turn
        bool canPreventArrest() const override {return true;} // Spy can always prevent arrest
        std::string role() const override{return "Spy";} // Returns the role of the player
    };



#endif // SPY_HPP