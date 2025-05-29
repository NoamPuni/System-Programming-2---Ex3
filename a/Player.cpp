#include <Player.hpp>
#include <stdexcept>
#include <iostream>
#include <string>
#include <memory>
#include <Game.hpp>

Player::Player(const std::string& name) // Constructor initializes the player's name and sets default values for coins and status flags
    : name(name), coins(0), is_sanctioned(false), is_alive(true), is_my_turn(false), is_last_one_arrested(false), is_prevented_from_arresting(false) {}
//--------------------------------------------------------------------------------------------------------------------
    std::string Player::getName() const {
    return name;
}
int Player::getCoins() const {
    return coins;
}

bool Player::isSanctioned() const {
    return is_sanctioned;
}

bool Player::isAlive() const {
    return is_alive;
}

bool Player::isMyTurn() const {
    return is_my_turn;
}

bool Player::isLastOneArrested() const {
    return is_last_one_arrested;
}

bool Player::isPreventedFromArresting() const {
    return is_prevented_from_arresting;
}
//--------------------------------------------------------------------------------------------------------------------
void Player::setCoins(int newCoins) { // Adds or subtracts coins from the player, ensuring that the total cannot go below zero
    coins += newCoins;
    if (coins < 0) {
        coins -= newCoins;
        throw std::runtime_error("Coins cannot be negative.");
    }
}

void Player::gotArrested(bool flag) {// Marks if the player is arrested or not, setting the last arrested flag accordingly
    is_last_one_arrested = flag;
}

void Player::sanctionMe() {
    is_sanctioned = true;
    setSanctionTurns(1); // הגדרת משך החרם (למשל, לתור אחד כברירת מחדל)
}

void Player::eliminateMe() { // eliminates the player from the game
    is_alive = false;
}

void Player::setTurn(bool my_turn) {// Sets whether it is the player's turn, allowing them to perform actions 
    is_my_turn = my_turn;
    if (is_my_turn) {
        onBeginTurn(); // Call onBeginTurn if it's the player's turn
    }
}

void Player::setSanctionTurns(int turns) {
    sanctionTurnsRemaining = turns;
    is_sanctioned = (turns > 0);
}

//--------------------------------------------------------------------------------------------------------------------
void Player::onBeginTurn() {
    if (sanctionTurnsRemaining > 0) {
        sanctionTurnsRemaining--;
        if (sanctionTurnsRemaining == 0) {
            is_sanctioned = false;
            std::cout << name << " is released from sanction." << std::endl;
        } else {
            std::cout << name << " is still sanctioned for this turn." << std::endl;
        }
    }  
    if (coins >= 10)
    {
        std::cout << name << " has too many coins and must coup or lose coins."<< std::endl;
    }
}
void Player::gather() {
    if (!is_my_turn) {
        throw std::runtime_error(name + " tried to gather out of turn.");
    }
    if (is_sanctioned) {
        throw std::runtime_error(name + " is sanctioned and can't gather.");
    }
    else {
        setCoins(1);
    }
}

void Player::tax(Game& game) {
    if (!is_my_turn) {
        throw std::runtime_error(name + " tried to tax out of turn.");
    }
    if (is_sanctioned) {
        throw std::runtime_error(name + " is sanctioned and can't tax.");
    }
    else {
        setCoins(2); // default tax amount; governor override to earn 3
        
        // Record for potential blocking
        game.recordTax(this);
        std::cout << name << " performed tax (can be blocked by Governor)" << std::endl;
    } //Governor can block $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
}

void Player::bribe(Game& game) {
    if (coins < 4) {
        throw std::runtime_error(name + " does not have enough coins to bribe.");
    }
    setCoins(-4);
    //record the bribe action in the game for potential blocking by Judge
    game.recordBribe(this);
    std::cout << name << " performed bribe (can be blocked by Judge)" << std::endl;
    // judge can undo, the coins doesn't return to the player $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
} //if Judge didn't undo -gives the player another 2 turns

bool Player::arrest(Player* target, Game& game) {
    if (isPreventedFromArresting()) {
        throw std::runtime_error("Spy prevented the arrest.");
    }

    if (!target->isAlive()) {
        throw std::runtime_error("Cannot arrest a non-active player.");
    }

    if (target->isLastOneArrested()) {
        throw std::runtime_error("Player cannot be arrested twice in a row.");
    }

    // ננסה לבצע את פעולת המעצר בפועל (עשויה לזרוק חריגה אם אין מספיק מטבעות)
    target->onArrestedBy(*this); // אם נכשל - תיזרק חריגה

    // אם הגענו לכאן, סימן שהמעצר הצליח => עכשיו נעדכן את מצב המשחק
    game.clearLastArrestedFlag();
    target->gotArrested(true);

    return true;
}

bool Player::sanction(Player* target) {
    // 1. בדיקות מקדימות שאינן תלויות במצב היעד
    if (!target) {
        throw std::runtime_error("Target player for sanction cannot be null.");
    }
    if (this == target) { // מניעת חרם עצמי
        throw std::runtime_error("Cannot sanction yourself.");
    }
    if (!target->isAlive()) {
        throw std::runtime_error("Can't sanction a non-active player.");
    }
    if (coins < 3) { // בדיקת עלות
        throw std::runtime_error(name + " doesn't have enough coins to sanction (needs 3).");
    }

    // 2. ניסיון להחיל את החרם על היעד.
    //    onSanctionedBy אמורה לזרוק חריגה אם היעד כבר מוחרם או לא ניתן להחרים אותו מסיבה אחרת.
    target->onSanctionedBy(*this); // קריאה זו תפעיל את הלוגיקה של היעד, כולל בדיקה אם הוא כבר מוחרם.

    // 3. אם onSanctionedBy לא זרקה חריגה, החרם נחשב כמוצלח.
    //    רק עכשיו מפחיתים את המטבעות מהשחקן המבצע.
    //    בהנחה ש-setCoins(int newCoins) היא פונקציה שקובעת את מספר המטבעות באופן מוחלט:
    this->setCoins(this->getCoins() - 3);
    return true; // החלת החרם הצליחה
}

bool Player::coup(Player* target, Game& game) {
    if (!target->isAlive()) {
        throw std::runtime_error("Cannot coup a non-active player.");
    }

    if (coins < 7) {
        throw std::runtime_error(name + " doesn't have enough coins for a coup.");
    }

    setCoins(-7);
    target->eliminateMe();
    // General can block the coup, if the coup is blocked, the target remains in the game and the player loses 7 coins
    // If the coup is successful, the target is eliminated from the game $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$44444
    // Record for potential blocking
    game.recordCoup(this, target);
    std::cout << name << " performed coup on " << target->getName() 
              << " (can be blocked by General)" << std::endl;
    return true;
}
//--------------------------------------------------------------------------------------------------------------------

bool Player::canBlockCoup() const {
    return false; // Default implementation, can be overridden by specific roles
}
bool Player::canUndoBribe() const {
    return false; // Default implementation, can be overridden by specific roles
}
bool Player::canBlockTax() const {
    return false; // Default implementation, can be overridden by specific roles
}
bool Player::canPreventArrest() const {
    return false; // Default implementation, can be overridden by specific roles
}
void Player::onSanctionedBy(Player& by) {
    // פונקציה זו נקראת על השחקן "היעד" של החרם.
    // 'by' הוא השחקן שמבצע את החרם.

    if (is_sanctioned) { // בדיקה אם היעד כבר מוחרם
        throw std::runtime_error(name + " is already sanctioned.");
    }

    // אם לא מוחרם, החל את החרם.
    this->sanctionMe(); // קריאה ל-sanctionMe כדי להגדיר את מצב החרם באופן עקבי.
                        // פונקציות שיורשות יכולות לדרוס את onSanctionedBy להוספת התנהגות ספציפית לתפקיד,
                        // אך עדיין יכולות לקרוא ל-Player::onSanctionedBy(by) או ישירות ל-sanctionMe().
}
void Player::onArrestedBy(Player& attacker) {
    // Default implementation does nothing, can be overridden by specific roles
     if (coins < 1) {
        throw std::runtime_error("Not enough coins to be arrested.");
    }
    this->setCoins(-1);       // player loses 1 coin when arrested
    attacker.setCoins(1);     // the attacker gains 1 coin
}

void Player::releaseSanction() {
    is_sanctioned = false;
    sanctionTurnsRemaining = 0;
    std::cout << name << " is released from sanction." << std::endl;
}