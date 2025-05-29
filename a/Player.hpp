// Player.hpp
#pragma once
#include <string>
#include <memory>
#include <stdexcept>

class Game; // forward declaration


class Player {
protected:
    std::string name;
    int coins = 0;
    bool is_sanctioned = false;
    bool is_alive = true;
    bool is_my_turn = false;
    bool is_last_one_arrested = false;
    bool is_prevented_from_arresting = false; 

    // Blocking system 
    Player* lastActionPlayer = nullptr;
    Player* coupTarget = nullptr; // for blocking coup
    bool can_Block_Bribe = false;
    bool can_Block_Tax = false;
    bool can_Block_Coup = false;
    int sanctionTurnsRemaining = 0;
public:
    Player(const std::string& name);
    virtual ~Player() {}

    // getters
    std::string getName() const;
    int getCoins() const;
    bool isSanctioned() const;
    bool isAlive() const;
    bool isMyTurn() const;
    bool isLastOneArrested() const;
    bool isPreventedFromArresting() const; 

    //  setters and state changes
    void setCoins(int newCoins);  
    void gotArrested(bool flag = true);// Marks the player as arrested, default to true
    void sanctionMe();
    void eliminateMe();
    void setTurn(bool val);
    void gotPreventedFromArresting() { is_prevented_from_arresting = true; } // Marks the player as prevented from arresting
    void restoreFromElimination() { is_alive = true; }
    void releaseSanction();
    void setSanctionTurns(int turns=1);

    // actions that can be performed by the player
    virtual void onBeginTurn(); // called at the beginning of the player's turn, can be overridden by specific roles
    virtual void gather(Game& game); // any player can gather coins, unless sanctioned - Added Game& game parameter
    virtual int tax(Game& game);    // MODIFIED: Returns the amount of coins to be taxed. Doesn't add coins itself.
    virtual void bribe(Game& game);  // דיפולט: -4; Judge יכול לבטל
    virtual bool coup(Player* target, Game& game); // -7, מוציא שחקן מהמשחק; General can block
    virtual bool arrest(Player* target, Game& game); // רווח לעצמי, נזק למטרה ;spy can prevent
    virtual void sanction(Player* target, Game& game); // -3, מסנדק שחקן - Added Game& game parameter

    // יכולות מיוחדות שנבדקות במשחק
    virtual std::string role() const = 0; // returns the role of the player, e.g., "Governor", "Judge", etc.

    // special abilities that can be overridden by specific roles
    virtual bool canBlockCoup() const;
    virtual bool canUndoBribe() const;
    virtual bool canBlockTax() const;
    virtual bool canPreventArrest() const;
    virtual void onSanctionedBy(Player& by, Game& game); // Added Game& game
    virtual void onArrestedBy(Player& attacker, Game& game); // Added Game& game
};