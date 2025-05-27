#ifndef JUDGE_HPP
#define JUDGE_HPP

#include "Player.hpp"
#include <string>


    class Judge : public Player {
    public:
        Judge(const std::string& name);
        ~Judge() override = default;

        // can undo a bribe
        void undoBribe(Player& bribingPlayer);

        // override methods
        std::string role() const override { return "Judge"; } // Returns the role of the player
        void onSanctionedBy(Player& attacker) override;
        bool canUndoBribe() const override { return true; } // Judge can undo a bribe
    };


#endif // JUDGE_HPP