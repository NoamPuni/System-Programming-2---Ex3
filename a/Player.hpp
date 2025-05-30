#pragma once
#include <string>
#include <memory>
#include <stdexcept>

class Game; // Forward declaration of the Game class.

class Player {
protected:
    std::string name; // The player's name.
    int coins = 0; // The player's current coin count.
    bool is_sanctioned = false; // Flag indicating if the player is currently sanctioned.
    bool is_alive = true; // Flag indicating if the player is alive in the game.
    bool is_my_turn = false; // Flag indicating if it's currently this player's turn.
    bool is_last_one_arrested = false; // Flag indicating if this player was the last one to be arrested.
    bool is_prevented_from_arresting = false; // Flag indicating if this player is prevented from arresting this turn.

    int sanctionTurnsRemaining = 0; // Number of turns remaining for sanction.

public:
    Player(const std::string& name); // Constructor: Initializes a new player with a given name.
    virtual ~Player() {} // Destructor: Virtual to ensure proper cleanup for derived classes.

    // Getters
    std::string getName() const; // Returns the player's name.
    int getCoins() const; // Returns the player's current coin count.
    bool isSanctioned() const; // Checks if the player is currently sanctioned.
    bool isAlive() const; // Checks if the player is alive.
    bool isMyTurn() const; // Checks if it's currently this player's turn.
    bool isLastOneArrested() const; // Checks if this player was the last one to be arrested.
    bool isPreventedFromArresting() const; // Checks if this player is prevented from performing an arrest this turn.

    // Setters and state changes
    void setCoins(int newCoins); // Sets the player's coin count.
    void gotArrested(bool flag = true); // Marks the player as arrested (or not).
    void sanctionMe(); // Sanctions the player.
    void eliminateMe(); // Eliminates the player from the game.
    void setTurn(bool val); // Sets whether it is this player's turn.
    void gotPreventedFromArresting(); // Marks the player as prevented from arresting.
    void restoreFromElimination(); // Restores the player from elimination.
    void releaseSanction(); // Releases the player from sanction.
    void setSanctionTurns(int turns = 1); // Sets the number of turns a player will be sanctioned.

    // Actions that can be performed by the player
    virtual void onBeginTurn(); // Called at the beginning of the player's turn.
    virtual void gather(Game& game); // Allows the player to gather coins.
    virtual bool arrest(Player* target, Game& game); // Allows the player to attempt an arrest.
    virtual void sanction(Player* target, Game& game); // Allows the player to sanction another player.
    virtual int tax(Game& game); // Returns the amount of coins a tax action would yield.
    virtual void bribe(Game& game); // Allows the player to bribe.
    virtual bool coup(Player* target, Game& game); // Allows the player to attempt a coup.

    // Special abilities checked by the game
    virtual std::string role() const = 0; // Returns the player's role (pure virtual).

    // Special abilities that can be overridden by specific roles
    virtual bool canBlockCoup() const; // Checks if the player can block a coup.
    virtual bool canUndoBribe() const; // Checks if the player can undo a bribe.
    virtual bool canBlockTax() const; // Checks if the player can block a tax.
    virtual bool canPreventArrest() const; // Checks if the player can prevent an arrest.
    virtual void onSanctionedBy(Player& by, Game& game); // Handles the effects of being sanctioned by another player.
    virtual void onArrestedBy(Player& attacker, Game& game); // Handles the effects of being arrested by an attacker.
};