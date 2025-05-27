#ifndef BARON_HPP
#define BARON_HPP

#include "Player.hpp"
#include <string>


class Baron : public Player {
public:
    Baron(const std::string& name);
    ~Baron() override = default;

    void invest(); // Baron can invest
    std::string Baron::role() const {
        return "Baron"; // Returns the role of the player
    }
    //override methods
    void onSanctionedBy(Player& attacker) override;
};

#endif // BARON_HPP