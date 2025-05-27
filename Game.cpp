#include "Game.hpp"
#include <stdexcept>
#include <algorithm>
#include <iostream>

Game::Game() : currentTurn(0), gameEnded(false) {
    // Constructor initializes turn index to 0 and game as not ended
}

Game::~Game() {
    // Destructor - clean up all player pointers
    for (Player* player : _players) {
        delete player;
    }
    _players.clear();
}

void Game::addPlayer(Player* player) {
    if (player == nullptr) {
        throw std::invalid_argument("Cannot add null player to game");
    }
    
    if (gameEnded) {
        throw std::runtime_error("Cannot add players to an ended game");
    }
    
    _players.push_back(player);
    
    /**If this is the first player, set their turn to true
    if (_players.size() == 1) {
        player->setTurn(true);
    }*/
}

void Game::nextTurn() {
    if (gameEnded) {
        throw std::runtime_error("Game has already ended");
    }
    
    if (_players.empty()) {
        throw std::runtime_error("No players in game");
    }
    // check if there are extra turns remaining to the current player
    if (extraTurnsRemaining > 0) {
        extraTurnsRemaining--;
        std::cout << _players[currentTurn]->getName() 
                  << " continues with extra turn! " 
                  << extraTurnsRemaining << " extra turns remaining." << std::endl;
        
        // new turn so new possible actions and blocking
        clearBlockableActions();
        
        // start the player's new turn
        _players[currentTurn]->onBeginTurn();
        return; // Don't switch players
    }

    // If no extra turns, continue normally

    // Set current player's turn to false
    _players[currentTurn]->setTurn(false);

    // Reset arrest flags for all players except the one who was just arrested
    if (_players[currentTurn]->isLastOneArrested()) {
        for (size_t i = 0; i < _players.size(); ++i) {
            if (i != currentTurn) {
                // Reset the last arrested flag for all other players
                _players[i]->gotArrested(false);
            }
        }
    }
    
    // Find next alive player
    size_t originalTurn = currentTurn;
    do {
        currentTurn = (currentTurn + 1) % _players.size();
        
        // Prevent infinite loop if no players are alive
        if (currentTurn == originalTurn) {
            // Check if current player is alive, if not, game should end
            if (!_players[currentTurn]->isAlive()) {
                gameEnded = true;
                throw std::runtime_error("No alive players remaining");
            }
            break;
        }
    } while (!_players[currentTurn]->isAlive());
    
    // Set new current player's turn to true
    _players[currentTurn]->setTurn(true);
    
    // Check if only one player remains alive
    int aliveCount = 0;
    for (Player* player : _players) {
        if (player->isAlive()) {
            aliveCount++;
        }
    }
    
    if (aliveCount <= 1) {
        gameEnded = true;
    }
    // Clear blockable actions when switching to new player
    clearBlockableActions();

    // Check if current player has 10+ coins and must coup
    if (_players[currentTurn]->getCoins() >= 10) {
        std::cout << _players[currentTurn]->getName() 
                  << " has " << _players[currentTurn]->getCoins() 
                  << " coins and must perform a coup!" << std::endl;
    }
    
}

std::string Game::turn() const {
    if (gameEnded) {
        throw std::runtime_error("Game has ended, no current turn");
    }
    
    if (_players.empty()) {
        throw std::runtime_error("No players in game");
    }
    
    std::cout << "Current turn: " << currentTurn << std::endl;
    return _players[currentTurn]->getName();
}

std::vector<std::string> Game::players() const {
    std::vector<std::string> activePlayerNames;
    
    for (const Player* player : _players) {
        if (player->isAlive()) {
            activePlayerNames.push_back(player->getName());
        }
    }
    
    return activePlayerNames;
}

std::string Game::winner() const {
    if (!gameEnded) {
        throw std::runtime_error("Game is still active, no winner yet");
    }
    
    // Find the single alive player
    Player* winner = nullptr;
    int aliveCount = 0;
    
    for (Player* player : _players) {
        if (player->isAlive()) {
            winner = player;
            aliveCount++;
        }
    }
    
    if (aliveCount == 0) {
        throw std::runtime_error("No players alive - game ended in a draw");
    }
    
    if (aliveCount > 1) {
        throw std::runtime_error("Game marked as ended but there are more than 1 players still alive");
    }
    
    return winner->getName();
}

void Game::recordBribe(Player* player) {
    lastActionPlayer = player;
    canBlockBribe = true;
    giveExtraTurns(2);
    std::cout << player->getName() << " gets 2 extra turns from bribe!" << std::endl;
}

void Game::giveExtraTurns(int turns = 2) {
    extraTurnsRemaining += turns;
}

void Game::recordTax(Player* player) {
    lastActionPlayer = player;
    canBlockTax = true;
}

void Game::recordCoup(Player* player, Player* target) {
    lastActionPlayer = player;
    canBlockCoup = true;
    // יכול לשמור גם את המטרה אם צריך
}

bool Game::tryBlockBribe(Player* blocker) {
    if (!canBlockBribe || !blocker->canUndoBribe()) {
        return false;
    }
    
    // Undo the bribe
    lastActionPlayer->setCoins(4); // Return the 4 coins
    // cancel the extra turns
    extraTurnsRemaining = 0;
    std::cout << blocker->getName() << " blocked " << lastActionPlayer->getName() 
              << "'s bribe!" << std::endl;
    
    canBlockBribe = false;
    return true;
}

bool Game::tryBlockTax(Player* blocker) {
    if (!canBlockTax || !blocker->canBlockTax()) {
        return false;
    }
    
    // Undo the tax
    lastActionPlayer->setCoins(-2); // Remove the gained coins
    std::cout << blocker->getName() << " blocked " << lastActionPlayer->getName() 
              << "'s tax!" << std::endl;
    
    canBlockTax = false;
    return true;
}

bool Game::tryBlockCoup(Player* blocker) {
    if (!canBlockCoup || !blocker->canBlockCoup()) {
        return false;
    }
    
    // Undo the coup - this is more complex, need to restore eliminated player
    lastActionPlayer->setCoins(7); // Return the 7 coins
    // Need to restore target player - need to store target reference
    std::cout << blocker->getName() << " blocked " << lastActionPlayer->getName() 
              << "'s coup!" << std::endl;
    
    canBlockCoup = false;
    return true;
}

void Game::clearBlockableActions() {
    lastActionPlayer = nullptr;
    canBlockBribe = false;
    canBlockTax = false;
    canBlockCoup = false;
}

void Game::clearLastArrestedFlag() {
    for (Player* player : _players) {
        player->gotArrested(false);
    }
}
// דוגמת שימוש במשחק:
/*
// דוגמה 1: שוחד
player->bribe(game); // מיידי: -4 מטבעות + רישום לחסימה

// בכל זמן עד התור הבא, שופט יכול לחסום:
if (judge->canUndoBribe()) {
    bool blocked = game.tryBlockBribe(judge);
    if (blocked) {
        std::cout << "Bribe was blocked! 4 coins returned." << std::endl;
    }
}

// דוגמה 2: מס
player->tax(game); // מיידי: +2 מטבעות + רישום לחסימה

// מושל יכול לחסום:
if (governor->canBlockTax()) {
    bool blocked = game.tryBlockTax(governor);
    if (blocked) {
        std::cout << "Tax was blocked! 2 coins removed." << std::endl;
    }
}

// דוגמה 3: הפיכה
player->coup(target, game); // מיידי: -7 מטבעות, מטרה מחוסלת + רישום לחסימה

// גנרל יכול לחסום:
if (general->canBlockCoup()) {
    bool blocked = game.tryBlockCoup(general);
    if (blocked) {
        std::cout << "Coup was blocked! Target restored, 7 coins returned." << std::endl;
    }
}

// כשמתחיל התור הבא:
game.nextTurn(); // מנקה את כל האפשרויות לחסימה אוטומטית
*/

