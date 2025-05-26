#ifndef GENERAL_HPP
#define GENERAL_HPP

#include "Player.hpp"
#include <string>

namespace coup {

    class General : public Player {
    public:
        General(const std::string& name);
        ~General() override = default;

        // יכולת לחסום הפיכה
        // targetPlayer הוא מי שההפיכה מכוונת נגדו.
        // originalAttacker הוא מי שניסה לבצע את ההפיכה.
        void blockCoup(Player& targetPlayerOfCoup, Player& originalAttacker);

        // דריסה של התגובה למעצר
        void onArrestedBy(Player& attacker) override;
    };

} // namespace coup
#endif // GENERAL_HPP