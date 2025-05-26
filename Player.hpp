#pragma once
#include <string>
#include <memory>
#include <stdexcept>

class Player {
protected:
    std::string name;
    int coins = 0;
    bool is_sanctioned = false;
    bool is_alive = true;
    bool is_my_turn = false;
    bool is_last_one_arrested = false;
    bool is_prevented_from_arresting = false; 
public:
    Player(const std::string& name);

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
    void gotArrested();
    void sanctionMe();
    void eliminateMe();
    void setTurn(bool val);

    // actions that can be performed by the player
    virtual void gather(); // any player can gather coins, unless sanctioned
    virtual void tax();    // דיפולט: +2; Governor: +3; Governor can block
    virtual void bribe();  // דיפולט: -4; Judge יכול לבטל
    virtual void coup(std::shared_ptr<Player> target); // -7, מוציא שחקן מהמשחק; General can block
    virtual void arrest(std::shared_ptr<Player> target); // רווח לעצמי, נזק למטרה ;spy can prevent
    virtual void sanction(std::shared_ptr<Player> target); // -3, מסנדק שחקן

    // יכולות מיוחדות שנבדקות במשחק
    virtual std::string role() const= 0; // returns the role of the player, e.g., "Governor", "Judge", etc.

    // special abilities that can be overridden by specific roles
    virtual bool canBlockCoup() const;
    virtual bool canUndoBribe() const;
    virtual bool canBlockTax() const;
    virtual bool canPreventArrest() const;
    virtual void onSanctionedBy(Player& by);
    virtual void onArrestedBy(Player& by);

    virtual ~Player() = default;
};