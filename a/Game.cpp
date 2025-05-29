// Game.cpp
#include "Game.hpp"
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>

// הכללת מחלקות התפקידים
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"

Game::Game() : currentTurn(0), gameEnded(false), _winnerName(""), rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    // Constructor initializes turn index to 0, game as not ended, and winner name
    // Initialize random number generator with current time
}

Game::~Game() {
    // Destructor - clean up all player pointers
    for (Player* player : _players) {
        delete player;
    }
    _players.clear();
}

void Game::initializeGame(const std::vector<std::string>& playerNames) {
    // בדוק שמספר השחקנים תקין (2-6)
    if (playerNames.size() < 2) {
        throw std::invalid_argument("Game requires at least 2 players");
    }
    if (playerNames.size() > 6) {
        throw std::invalid_argument("Game allows maximum 6 players");
    }
    
    // בדוק שהמשחק עדיין לא התחיל
    if (!_players.empty()) {
        throw std::runtime_error("Game has already been initialized");
    }
    
    // רשימת התפקידים הזמינים
    std::vector<std::string> roles = {"Governor", "Baron", "Judge", "Spy", "General", "Merchant"};
    std::shuffle(roles.begin(), roles.end(), rng); // ערבב את התפקידים

    for (size_t i = 0; i < playerNames.size(); ++i) {
        Player* newPlayer = createPlayerWithRole(playerNames[i], roles[i]);
        _players.push_back(newPlayer);
    }
}

bool Game::canStartGame() const {
    return _players.size() >= 2 && _players.size() <= 6;
}

void Game::addPlayer(Player* player) {
    _players.push_back(player);
}

void Game::nextTurn() {
    // Clear any blockable actions from the previous turn
    clearBlockableActions();

    // Clear the "last arrested" flag from all players at the start of a new turn sequence
    clearLastArrestedFlag();

    // Handle extra turns for players who bribed
    if (extraTurnsRemaining > 0) {
        extraTurnsRemaining--;
        // The current player gets an extra turn, so we don't advance currentTurn
    } else {
        // Advance to the next living player
        size_t originalCurrentTurn = currentTurn;
        do {
            currentTurn = (currentTurn + 1) % _players.size();
        } while (!_players[currentTurn]->isAlive() && currentTurn != originalCurrentTurn); // Loop until alive player or back to start
        
        // If all players are eliminated except one, the game ends
        if (getAlivePlayerCount() <= 1) {
            gameEnded = true;
            for (Player* p : _players) {
                if (p->isAlive()) {
                    _winnerName = p->getName();
                    break;
                }
            }
        }
    }

    if (gameEnded) return;

    // Call onBeginTurn for the new current player
    _players[currentTurn]->onBeginTurn();
}

std::string Game::turn() const {
    if (gameEnded) {
        return "Game Over";
    }
    if (_players.empty()) {
        return "No players yet.";
    }
    if (currentTurn >= _players.size()) {
        return "Error: Invalid turn index."; // Should not happen
    }
    return _players[currentTurn]->getName() + "'s turn.";
}

std::vector<std::string> Game::players() const {
    std::vector<std::string> playerNames;
    for (const auto& player : _players) {
        playerNames.push_back(player->getName());
    }
    return playerNames;
}

std::string Game::winner() const {
    if (gameEnded) {
        return _winnerName;
    }
    return "No winner yet.";
}

Player* Game::createPlayerWithRole(const std::string& name, const std::string& role) {
    if (role == "Governor") {
        return new Governor(name);
    } else if (role == "Baron") {
        return new Baron(name);
    } else if (role == "Judge") {
        return new Judge(name);
    } else if (role == "Spy") {
        return new Spy(name);
    } else if (role == "General") {
        return new General(name);
    } else if (role == "Merchant") {
        return new Merchant(name);
    }
    throw std::invalid_argument("Unknown role: " + role);
}

// Blocking methods
void Game::recordBribe(Player* player) {
    lastActionPlayer = player;
    canBlockBribe = true;
}

// MODIFIED: Now takes amount as a parameter
void Game::recordTax(Player* player, int amount) {
    lastActionPlayer = player;
    lastTaxAmount = amount; // Store the amount to be taxed
    canBlockTax = true;
}

void Game::recordCoup(Player* player, Player* target) {
    lastActionPlayer = player;
    coupTarget = target;
    canBlockCoup = true;
}

bool Game::tryBlockBribe(Player* blocker) {
    if (!canBlockBribe || !blocker->canUndoBribe() || lastActionPlayer == nullptr) {
        return false;
    }
    // Implement bribe undo logic (e.g., return coins to bribing player)
    // For now, let's assume it just blocks the effect.
    std::cout << blocker->getName() << " blocked " << lastActionPlayer->getName() 
              << "'s bribe!" << std::endl;
    canBlockBribe = false;
    return true;
}

// MODIFIED: If block succeeds, coins are NOT added.
bool Game::tryBlockTax(Player* blocker) {
    if (!canBlockTax || !blocker->canBlockTax() || lastActionPlayer == nullptr) {
        return false;
    }
    // אם נחסם, פשוט נמחק את הרישום כך שלא יתווספו מטבעות
    std::cout << blocker->getName() << " (Governor) BLOCKED " << lastActionPlayer->getName() 
              << "'s tax!" << std::endl;
    lastTaxAmount = 0; // ✅ זה כבר קיים וטוב
    canBlockTax = false;
    return true;
}

bool Game::tryBlockCoup(Player* blocker) {
    if (!canBlockCoup || !blocker->canBlockCoup()) {
        return false;
    }
    
    // restore the target player from elimination
    if (coupTarget != nullptr) {
        coupTarget->restoreFromElimination();
    }
    // lastActionPlayer->setCoins(lastActionPlayer->getCoins() + 7); // This was previously problematic, needs re-evaluation if coup deducts early
    // For now, assuming coup already deducted the coins. If it was blocked, they should be returned.
    if (lastActionPlayer != nullptr) {
        // Need to add 7 coins back to the player who performed the coup if it was blocked
        lastActionPlayer->setCoins(7); // Assuming coup cost 7
    }

    std::cout << blocker->getName() << " blocked " << lastActionPlayer->getName() 
              << "'s coup!" << std::endl;
    
    canBlockCoup = false;
    return true;
}

void Game::clearBlockableActions() {
    lastActionPlayer = nullptr;
    coupTarget = nullptr; // Also clear coup target
    lastTaxAmount = 0; // Clear pending tax amount
    canBlockBribe = false;
    canBlockTax = false;
    canBlockCoup = false;
}

void Game::clearLastArrestedFlag() {
    for (Player* player : _players) {
        player->gotArrested(false);
    }
}

size_t Game::getAlivePlayerCount() const {
    size_t count = 0;
    for (const Player* player : _players) {
        if (player->isAlive()) {
            count++;
        }
    }
    return count;
}

Player* Game::getCurrentPlayer() const {
    if (_players.empty() || gameEnded) {
        return nullptr;
    }
    
    // Find the next alive player if currentTurn is on an eliminated player
    // This assumes nextTurn() already handles advancing to an alive player,
    // but this is a safeguard for direct access.
    size_t tempTurn = currentTurn;
    while (!_players[tempTurn]->isAlive()) {
        tempTurn = (tempTurn + 1) % _players.size();
        if (tempTurn == currentTurn) { // Looped through all and none are alive (should be handled by gameEnded)
            return nullptr;
        }
    }
    return _players[tempTurn];
}

void Game::giveExtraTurns(int turns) {
    extraTurnsRemaining += turns;
}

std::vector<std::pair<std::string, std::string>> Game::getPlayersWithRoles() const {
    std::vector<std::pair<std::string, std::string>> playerRoles;
    for (const Player* player : _players) {
        playerRoles.push_back({player->getName(), player->role()});
    }
    return playerRoles;
}
void Game::recordAction(Player* performer, const std::string& actionType, Player* target) {
    _lastActionPerformer = performer;
    _lastActionType = actionType;
    _lastActionTarget = target;

    // Reset all block flags and set only relevant ones
    canBlockBribeFlag = false;
    canBlockTaxFlag = false;
    canBlockCoupFlag = false; // Initialize to false for all actions

    if (actionType == "bribe") {
        for (Player* p : _players) {
            if (p->isAlive() && p != performer && p->canUndoBribe()) {
                canBlockBribeFlag = true;
                break;
            }
        }
    } else if (actionType == "tax") {
        for (Player* p : _players) {
            if (p->isAlive() && p != performer && p->canBlockTax()) {
                canBlockTaxFlag = true;
                break;
            }
        }
    }
    // Add similar logic for other blockable actions (e.g., "coup")
}

void Game::clearLastAction() {
    _lastActionPerformer = nullptr;
    _lastActionType = "";
    _lastActionTarget = nullptr;
    canBlockBribeFlag = false;
    canBlockTaxFlag = false;
    canBlockCoupFlag = false;
}

// מימוש ל-tryBlock
Player* Game::tryBlock(const std::string& actionType, Player* performer, Player* target) {
    if (actionType == "bribe" && canBlockBribeFlag) {
        // Iterate through all players to find a Judge who can block this bribe
        for (Player* p : _players) {
            if (p->isAlive() && p != performer && p->canUndoBribe()) { // A player cannot block their own action
                return p; // Found a Judge who can block
            }
        }
    } else if (actionType == "tax" && canBlockTaxFlag) {
        // Iterate through all players to find a Governor who can block this tax
        for (Player* p : _players) {
            if (p->isAlive() && p != performer && p->canBlockTax()) {
                return p; // Found a Governor who can block
            }
        }
    }
    // Add logic for other blockable actions here (e.g., "coup" for General)

    return nullptr; // No one can block this action
}