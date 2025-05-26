#ifndef MERCHANT_HPP
#define MERCHANT_HPP

#include "Player.hpp"
#include <string>

namespace coup {

    class Merchant : public Player {
    public:
        Merchant(const std::string& name);
        ~Merchant() override = default;

        // דריסה של הפעולה בתחילת תור
        //void onBeginTurn() override;

        // דריסה של התגובה למעצר
        void onArrestedBy(Player& attacker) override;
    };

} // namespace coup
#endif // MERCHANT_HPP