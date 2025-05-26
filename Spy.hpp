#ifndef SPY_HPP
#define SPY_HPP

#include "Player.hpp"
#include <string>

    class Spy : public Player {
    public:
        Spy(const std::string& name);
        ~Spy() override = default;

        // היכולת המיוחדת של המרגל - אינה פעולה בתור
        void revealCoins(Player& targetPlayer) const; // חושף מטבעות של שחקן אחר
        void preventArrest(Player& targetPlayer);    // מונע מ-targetPlayer להשתמש במעצר בתורו הבא

        // המרגל לא דורס פעולות בסיסיות כמו tax או foreign_aid בצורה שונה,
        // אלא אם כן יש לו תגובה ספציפית להתקפה כלשהי.
        // על פי התיאור, אין לו תגובות כאלה.
    };



#endif // SPY_HPP