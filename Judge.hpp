#ifndef JUDGE_HPP
#define JUDGE_HPP

#include "Player.hpp"
#include <string>


class Judge : public Player {
public:
    Judge(const std::string& name); // Constructor for the Judge class.

    bool undoBribe(Player& bribingPlayer); // Allows the Judge to undo a bribe.

    // Override methods
    std::string role() const override; // Returns the role of the player.
    void onSanctionedBy(Player& attacker, Game& game) override; // Defines how the Judge reacts when sanctioned.
    bool canUndoBribe() const override { return true; } // Judge can undo a bribe.
};


#endif // JUDGE_HPP