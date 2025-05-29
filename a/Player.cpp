// Player.cpp
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
void Player::setCoins(int newCoins) {
    std::cout << "setCoins called for " << name << " with " << newCoins << " coins." << std::endl;
    // בדוק אם הניסיון להפחית מטבעות יוריד את הסכום מתחת לאפס
    if (coins + newCoins < 0) {
        throw std::runtime_error("Coins cannot be negative.");
    }
    coins += newCoins;
}

void Player::gotArrested(bool flag) {
    is_last_one_arrested = flag;
}

void Player::sanctionMe() {
    is_sanctioned = true;
    sanctionTurnsRemaining = 1; // Default to 1 turn of sanction
}

void Player::eliminateMe() {
    is_alive = false;
}

void Player::setTurn(bool val) {
    is_my_turn = val;
}

void Player::releaseSanction() {
    is_sanctioned = false;
    sanctionTurnsRemaining = 0;
}

void Player::setSanctionTurns(int turns) {
    sanctionTurnsRemaining = turns;
}

// actions that can be performed by the player
void Player::onBeginTurn() {
    if (sanctionTurnsRemaining > 0) {
        sanctionTurnsRemaining--;
        if (sanctionTurnsRemaining == 0) {
            releaseSanction();
        }
    }
    is_prevented_from_arresting = false; // Reset prevention at the start of turn
}

void Player::gather(Game& game) { // Added Game& game parameter
    if (is_sanctioned) {
        throw std::runtime_error(name + " is sanctioned and can't gather.");
    }
    setCoins(1); // Use the modified setCoins
}

// MODIFIED: This function now returns the amount of coins the tax *would* give, but does not add them.
int Player::tax(Game& game) { 
    if (is_sanctioned) {
        throw std::runtime_error(name + " is sanctioned and can't tax.");
    }
    return 2; // Default player taxes 2 coins
}

void Player::bribe(Game& game) {
    if (getCoins() < 4) {
        throw std::runtime_error(name + " does not have enough coins for bribe (needs 4).");
    }
    setCoins(-4);
    game.giveExtraTurns(); // Give extra turns for bribing
}

bool Player::coup(Player* target, Game& game) {
    if (getCoins() < 7) {
        throw std::runtime_error(name + " does not have enough coins for coup (needs 7).");
    }
    if (!target->isAlive()) {
        throw std::runtime_error(target->getName() + " is already eliminated.");
    }
    
    setCoins(-7); // Deduct coins immediately
    target->eliminateMe(); // Eliminate target immediately
    // game.recordCoup(this, target); // Record for blocking if needed, but coup is usually unblockable in classic Coup

    return true; // Coup successful
}

bool Player::arrest(Player* target, Game& game) {
    if (!target->isAlive()) {
        throw std::runtime_error(target->getName() + " is already eliminated.");
    }
    if (is_prevented_from_arresting) {
        throw std::runtime_error(name + " is prevented from arresting this turn.");
    }
    
    // Check if target was recently arrested
    if (target->isLastOneArrested()) {
        throw std::runtime_error(target->getName() + " was recently arrested and cannot be arrested again.");
    }

    try {
        target->onArrestedBy(*this, game); // Target handles the arrest effect (losing coin, possibly prevention)
        setCoins(1); // Attacker gains 1 coin
        target->gotArrested(); // Mark target as recently arrested
        game.clearLastArrestedFlag(); // This needs to be called in game.nextTurn() not here
        return true;
    } catch (const std::exception& e) {
        // If target doesn't have enough coins, or any other arrest specific issue, it will throw here
        throw std::runtime_error("Arrest failed: " + std::string(e.what()));
    }
    return false;
}


void Player::sanction(Player* target, Game& game) { // Added Game& game
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
    target->onSanctionedBy(*this, game); // Target handles the sanction effect
}


// Special abilities
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

void Player::onSanctionedBy(Player& by, Game& game) { // Added Game& game
    if (is_sanctioned) {
        throw std::runtime_error(name + " is already sanctioned.");
    }
    this->sanctionMe();
}

void Player::onArrestedBy(Player& attacker, Game& game) { // Added Game& game
    if (coins < 1) {
        throw std::runtime_error("Not enough coins to be arrested.");
    }
    this->setCoins(-1); // Player loses 1 coin
}
