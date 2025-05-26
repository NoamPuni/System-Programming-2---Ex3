#ifndef BARON_HPP
#define BARON_HPP

#include "Player.hpp"
#include <string>

namespace coup {

    class Baron : public Player {
    public:
        Baron(const std::string& name);
        ~Baron() override = default;

        void invest(); // פעולת ההשקעה הייחודית

        // דריסה של התגובה לחרם
        void onSanctionedBy(Player& attacker) override;
    };

} // namespace coup
#endif // BARON_HPP