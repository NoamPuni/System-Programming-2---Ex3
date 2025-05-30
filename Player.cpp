#include <Player.hpp>
#include <stdexcept>
#include <iostream>
#include <string>
#include <memory>
#include <Game.hpp>

// Constructor initializes the player's name and sets default values for coins and status flags.
Player::Player(const std::string& name) 
    : name(name), coins(0), is_sanctioned(false), is_alive(true), is_my_turn(false), is_last_one_arrested(false), is_prevented_from_arresting(false) {}

// Returns the player's name.
std::string Player::getName() const {
    return name;
}

// Returns the player's current coin count.
int Player::getCoins() const {
    return coins;
}

// Checks if the player is currently sanctioned.
bool Player::isSanctioned() const {
    return is_sanctioned;
}

// Checks if the player is alive.
bool Player::isAlive() const {
    return is_alive;
}

// Checks if it's currently this player's turn.
bool Player::isMyTurn() const {
    return is_my_turn;
}

// Checks if this player was the last one to be arrested.
bool Player::isLastOneArrested() const {
    return is_last_one_arrested;
}

// Checks if this player is prevented from performing an arrest this turn.
bool Player::isPreventedFromArresting() const {
    return is_prevented_from_arresting;
}

// Sets the player's coin count, adding or subtracting from the current amount.
void Player::setCoins(int newCoins) {
    if (coins + newCoins < 0) {
        throw std::runtime_error("Coins cannot be negative.");
    }
    coins += newCoins;
}

// Sets the 'last arrested' flag for the player.
void Player::gotArrested(bool flag) {
    is_last_one_arrested = flag;
}

// Sanctions the player for a default duration.
void Player::sanctionMe() {
    is_sanctioned = true;
    sanctionTurnsRemaining = 1; // Default to 1 turn of sanction.
}

// Marks the player as eliminated (no longer alive).
void Player::eliminateMe() {
    is_alive = false;
}

// Sets whether it is this player's turn.
void Player::setTurn(bool val) {
    is_my_turn = val;
}

// Releases the player from sanction.
void Player::releaseSanction() {
    is_sanctioned = false;
    sanctionTurnsRemaining = 0;
}

// Sets the number of turns a player will be sanctioned.
void Player::setSanctionTurns(int turns) {
    sanctionTurnsRemaining = turns;
}

// Actions that can be performed by the player.
// Called at the beginning of the player's turn to update their status.
void Player::onBeginTurn() {
    if (sanctionTurnsRemaining > 0) {
        sanctionTurnsRemaining--;
        if (sanctionTurnsRemaining == 0) {
            releaseSanction();
        }
    }
    is_prevented_from_arresting = false; // Reset prevention at the start of turn.
}

// Allows the player to gather one coin.
void Player::gather(Game& game) { 
    if (is_sanctioned) {
        throw std::runtime_error(name + " is sanctioned and can't gather.");
    }
    setCoins(1); // Use the modified setCoins.
}

// Returns the amount of coins a tax action would yield without adding them.
int Player::tax(Game& game) { 
    if (is_sanctioned) {
        throw std::runtime_error(name + " is sanctioned and can't tax.");
    }
    return 2; // Default tax is 2 coins for a regular player.
}

// Allows the player to bribe, deducting coins immediately.
void Player::bribe(Game& game) {
    if (getCoins() < 4) {
        throw std::runtime_error(name + " does not have enough coins to bribe (needs 4).");
    }
    setCoins(-4); // Deduct coins immediately.
}

// Attempts a coup against a target player.
bool Player::coup(Player* target, Game& game) {
    if (!target) {
        throw std::invalid_argument("Coup target cannot be null.");
    }
    if (getCoins() < 7) {
        throw std::runtime_error(name + " does not have enough coins for coup (needs 7).");
    }
    if (!target->isAlive()) {
        throw std::runtime_error(target->getName() + " is already eliminated.");
    }

    setCoins(-7); // Deduct coins immediately.
    target->eliminateMe(); // Mark target for elimination.
    return true; // Coup attempt was successful (coins deducted, target marked for elimination).
}

// Attempts to arrest a target player.
bool Player::arrest(Player* target, Game& game) {
    if (!target->isAlive()) {
        throw std::runtime_error(target->getName() + " is already eliminated.");
    }
    if (is_prevented_from_arresting) {
        throw std::runtime_error(name + " is prevented from arresting this turn.");
    }
    
    if (target->isLastOneArrested()) {
        throw std::runtime_error(target->getName() + " was recently arrested and cannot be arrested again.");
    }

    try {
        target->onArrestedBy(*this, game); // Target handles the arrest effect (losing coin, possibly prevention).
        setCoins(1); // Attacker gains 1 coin.
        target->gotArrested(); // Mark target as recently arrested.
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Arrest failed: " + std::string(e.what()));
    }
    return false;
}

// Allows the player to sanction a target player.
void Player::sanction(Player* target, Game& game) { 
    if (is_sanctioned) {
        throw std::runtime_error(name + " is sanctioned and can't sanction.");
    }
    if (getCoins() < 3) {
        throw std::runtime_error(name + " does not have enough coins for sanction (needs 3).");
    }
    if (!target->isAlive()) {
        throw std::runtime_error(target->getName() + " is already eliminated.");
    }
    if (target->isSanctioned()) {
        throw std::runtime_error(target->getName() + " is already sanctioned.");
    }

    setCoins(-3);
    target->onSanctionedBy(*this, game); // Target handles the sanction effect.
}

// Special abilities.
// Default implementation: Player cannot block a coup.
bool Player::canBlockCoup() const {
    return false; 
}

// Default implementation: Player cannot undo a bribe.
bool Player::canUndoBribe() const {
    return false; 
}

// Default implementation: Player cannot block a tax.
bool Player::canBlockTax() const {
    return false; 
}

// Default implementation: Player cannot prevent an arrest.
bool Player::canPreventArrest() const {
    return false; 
}

// Handles the effects of being sanctioned by another player.
void Player::onSanctionedBy(Player& by, Game& game) { 
    if (is_sanctioned) {
        throw std::runtime_error(name + " is already sanctioned.");
    }
    this->sanctionMe();
}

// Handles the effects of being arrested by an attacker.
void Player::onArrestedBy(Player& attacker, Game& game) { 
    if (coins < 1) {
        throw std::runtime_error("Not enough coins to be arrested.");
    }
    this->setCoins(-1); // Player loses 1 coin.
}