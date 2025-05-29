#ifndef BARON_HPP
#define BARON_HPP

#include "Player.hpp"
#include <string>


class Baron : public Player {
public:
    Baron(const std::string& name);


    void invest(); // Baron can invest
    std::string role() const override; // Returns the role of the player

    //override methods
    void onSanctionedBy(Player& attacker, Game& game) override;
};

#endif // BARON_HPP