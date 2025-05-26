#ifndef JUDGE_HPP
#define JUDGE_HPP

#include "Player.hpp"
#include <string>

namespace coup {

    class Judge : public Player {
    public:
        Judge(const std::string& name);
        ~Judge() override = default;

        // יכולת לבטל שוחד
        // bribingPlayer הוא מי שניסה לשלם שוחד
        void blockBribe(Player& bribingPlayer);

        // דריסה של התגובה לחרם
        void onSanctionedBy(Player& attacker) override;
    };

} // namespace coup
#endif // JUDGE_HPP