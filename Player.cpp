#include <Player.hpp>
#include <stdexcept>
#include <iostream>
#include <string>
#include <memory>

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
        throw std::runtime_error("Coins cannot be negative.");
    }
}

void Player::gotArrested() {// Marks the player as arrested, setting the last arrested flag to true
    is_last_one_arrested = true;
}

void Player::sanctionMe() {// Marks the player as sanctioned, preventing them from doing tax or gather
    is_sanctioned = true;
}

void Player::eliminateMe() { // eliminates the player from the game
    is_alive = false;
}

void Player::setTurn(bool my_turn) {// Sets whether it is the player's turn, allowing them to perform actions 
    is_my_turn = my_turn;
}

//--------------------------------------------------------------------------------------------------------------------

void Player::gather() {
    if (is_sanctioned) {
        throw std::runtime_error(name + " is sanctioned and can't gather.");
    }
    else {
        setCoins(1);
    }
}

void Player::tax() {
    if (is_sanctioned) {
        throw std::runtime_error(name + " is sanctioned and can't tax.");
    }
    else {
        setCoins(2); // default tax amount; governor override to earn 3
    } //Governor can block $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
}

void Player::bribe() { 
    if (coins < 4) {
        throw std::runtime_error(name + " does not have enough coins to bribe.");
    }
    setCoins(-4);
    // judge can undo, the coins doesn't return to the player $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
} //if Judge didn't undo -gives the player another 2 turns

void Player::arrest(std::shared_ptr<Player> target) { //spy can prevent
    //if spy prevented the arrest, do nothing
    if( isPreventedFromArresting() ) {
        throw std::runtime_error("Spy prevented so cannot arresting other players."); 
    }

    if (!target->isAlive()) {
        throw std::runtime_error("Can't arrest non active player.");
    }

    if (target->isLastOneArrested()) {
        throw std::runtime_error("player cannot be arrested twice in a row.");
    }
    target->gotArrested(); // Mark the target as arrested

    target->onArrestedBy(*this);
    this->setCoins(1);
    target->setCoins(-1);  // will be overridden by specific roles

    /*if (target->role() != "General") {// the 1 coin returns to the general(=do nothing);
        if (target->role() == "Merchant") {//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ using role()
            target->setCoins(-2);
            this->setCoins(0); // Merchant loses 2 coins, arresting player gains nothing
        }
        else {
            this->setCoins(1);
            target->setCoins(-1); // All other roles lose 1 coin, arresting player gains 1 coin
        }
    }*/

}

void Player::sanction(std::shared_ptr<Player> target) {
    if (!target->isAlive()) {
        throw std::runtime_error("Can't sanction non active player.");
    }

    if (coins < 3) {
        throw std::runtime_error(name + " doesn't have enough coins to sanction.");
    }

    setCoins(-3);
    target->sanctionMe();
    target->onSanctionedBy(*this); // will be overridden by specific roles
        
    /*if (target->role() == "Baron") { // If the target is a Baron, he gain 1 coin
        target->setCoins(1);
    }
    if (target->role() == "Judge") { // If the target is a Judge, the player who sanctioned him loses 1 more coin
        this->setCoins(-1);
    }*/
}

void Player::coup(std::shared_ptr<Player> target) {
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
    // Default implementation does nothing, can be overridden by specific roles
}
void Player::onArrestedBy(Player& by) {

    // Default implementation does nothing, can be overridden by specific roles
}
std::string Player::role() const {
    // Default implementation does nothing, can be overridden by derived classes
}

Player::~Player() = default; // Default destructor, can be overridden by derived classes if needed
