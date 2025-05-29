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
    std::vector<std::string> availableRoles = {
        "Governor", "Spy", "Baron", "General", "Judge", "Merchant"
    };
    
    // הכן מחולל מספרים אקראיים להגרלת תפקידים
    std::uniform_int_distribution<int> roleDist(0, availableRoles.size() - 1);
    
    std::cout << "Initializing game with " << playerNames.size() << " players:" << std::endl;
    
    // צור שחקנים עם תפקידים מוגרלים באקראי (כל שחקן מקבל תפקיד אקראי)
    for (const std::string& playerName : playerNames) {
        // הגרל תפקיד אקראי לכל שחקן
        int randomRoleIndex = roleDist(rng);
        const std::string& assignedRole = availableRoles[randomRoleIndex];
        
        Player* newPlayer = createPlayerWithRole(playerName, assignedRole);
        addPlayer(newPlayer);
        
        std::cout << "Player " << playerName << " assigned role: " << assignedRole << std::endl;
    }
    
    std::cout << "Game initialized successfully!" << std::endl;
    std::cout << "Turn order: ";
    for (size_t i = 0; i < _players.size(); ++i) {
        std::cout << _players[i]->getName();
        if (i < _players.size() - 1) std::cout << " -> ";
    }
    std::cout << std::endl;
    std::cout << "First turn: " << _players[0]->getName() << std::endl;
}

Player* Game::createPlayerWithRole(const std::string& name, const std::string& role) {
    if (role == "Governor") {
        return new Governor(name);
    } else if (role == "Spy") {
        return new Spy(name);
    } else if (role == "Baron") {
        return new Baron(name);
    } else if (role == "General") {
        return new General(name);
    } else if (role == "Judge") {
        return new Judge(name);
    } else if (role == "Merchant") {
        return new Merchant(name);
    } else {
        throw std::invalid_argument("Unknown role: " + role);
    }
}

bool Game::canStartGame() const {
    return _players.size() >= 2 && _players.size() <= 6 && !gameEnded;
}

std::vector<std::pair<std::string, std::string>> Game::getPlayersWithRoles() const {
    std::vector<std::pair<std::string, std::string>> playersInfo;
    
    for (const Player* player : _players) {
        if (player->isAlive()) {
            playersInfo.emplace_back(player->getName(), player->role());
        }
    }
    
    return playersInfo;
}

void Game::addPlayer(Player* player) {
    if (player == nullptr) {
        throw std::invalid_argument("Cannot add null player to game");
    }
    
    if (gameEnded) {
        throw std::runtime_error("Cannot add players to an ended game");
    }
    
    // בדוק שלא חורגים מ-6 שחקנים
    if (_players.size() >= 6) {
        throw std::runtime_error("Cannot add more than 6 players to the game");
    }
    
    _players.push_back(player);
    
    //If this is the first player, set their turn to true
    if (_players.size() == 1) {
        player->setTurn(true);
    }
}

void Game::nextTurn() {
    clearBlockableActions();
    clearLastArrestedFlag(); // Call this here to clear flags before next turn logic

    // Check for game end BEFORE proceeding with the turn or currentTurn logic
    size_t aliveCount = getAlivePlayerCount();
    if (aliveCount <= 1) {
        gameEnded = true;
        if (aliveCount == 1) {
            for (Player* p : _players) {
                if (p->isAlive()) {
                    _winnerName = p->getName();
                    break;
                }
            }
        } else { // aliveCount == 0, a draw or all eliminated
            _winnerName = "No one (All eliminated)";
        }
        // No need to throw runtime_error here; GUI will handle game end
        return; // Exit the function, game has ended
    }
    
    // If game has already ended (e.g., from a previous check in main_gui), prevent further action
    if (gameEnded) {
        return; 
    }

    // if extra turns are remaining, the current player gets another turn
    if (extraTurnsRemaining > 0) {
        extraTurnsRemaining--;
        // Stay on current player, and don't clear their is_my_turn flag
        // The current player's onBeginTurn will be called again when main_gui checks it
        _players[currentTurn]->onBeginTurn(); // Call onBeginTurn for the current player for their extra turn
        return;
    }

    // If no extra turns, continue normally
    // Set previous player's turn to false
    // Ensure currentTurn is valid before accessing _players[currentTurn]
    if (!_players.empty() && currentTurn < _players.size()) {
        _players[currentTurn]->setTurn(false);
    } else {
        // This case should ideally not be hit if aliveCount <= 1 check is robust
        // but as a safeguard or if game initialization failed.
        gameEnded = true;
        _winnerName = "Error: Game state invalid, no current player.";
        return;
    }

    // Find the next alive player
    size_t originalCurrentTurn = currentTurn;
    do {
        currentTurn = (currentTurn + 1) % _players.size();
        // Prevent infinite loop if no players are alive (already handled above but as a safeguard)
        if (currentTurn == originalCurrentTurn) {
            if (! _players[currentTurn]->isAlive()){ // if the only remaining player is also dead, then game ended
                gameEnded = true;
                _winnerName = "Error: No alive players found after turn progression.";
                return;
            }
            break; // Found the original player, meaning they are the only one left or an issue
        }
    } while (!_players[currentTurn]->isAlive());
    
    // Set new current player's turn to true
    _players[currentTurn]->setTurn(true);
    _players[currentTurn]->onBeginTurn(); // Call onBeginTurn for the new current player

    // Check if current player has 10+ coins and must coup
    if (_players[currentTurn]->getCoins() >= 10) {
        std::cout << _players[currentTurn]->getName() 
                    << " has " << _players[currentTurn]->getCoins() 
                    << " coins and must perform a coup!" << std::endl;
    }
}


std::string Game::turn() const {
    // If the game has ended, don't throw an error, main_gui will check isGameEnded()
    if (gameEnded) {
        return "Game Over"; // Or some other indication that there's no active turn
    }
    
    if (_players.empty()) {
        throw std::runtime_error("No players in game");
    }
    
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
        // Only return winner if game has officially ended
        return ""; // Or throw a specific error indicating game is not over
    }
    // If game has ended, _winnerName should have been set by nextTurn()
    return _winnerName;
}

void Game::recordBribe(Player* player) {
    lastActionPlayer = player;
    canBlockBribe = true;
    giveExtraTurns(2);
    std::cout << player->getName() << " gets 2 extra turns from bribe!" << std::endl;
}

void Game::giveExtraTurns(int turns) {
    extraTurnsRemaining += turns;
}

void Game::recordTax(Player* player) {
    lastActionPlayer = player;
    canBlockTax = true;
}

void Game::recordCoup(Player* player, Player* target) {
    lastActionPlayer = player;
    coupTarget = target;
    canBlockCoup = true;
}

bool Game::tryBlockBribe(Player* blocker) {
    if (!canBlockBribe || !blocker->canUndoBribe()) {
        return false;
    }
    
    // Undo the bribe
    lastActionPlayer->setCoins(lastActionPlayer->getCoins() + 4); // Return the 4 coins
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
    lastActionPlayer->setCoins(lastActionPlayer->getCoins() - 2); // Remove the gained coins
    std::cout << blocker->getName() << " blocked " << lastActionPlayer->getName() 
              << "'s tax!" << std::endl;
    
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
    lastActionPlayer->setCoins(lastActionPlayer->getCoins() + 7); // Return the 7 coins
    // Need to restore target player - need to store target reference
    std::cout << blocker->getName() << " blocked " << lastActionPlayer->getName() 
              << "'s coup!" << std::endl;
    
    canBlockCoup = false;
    return true;
}

void Game::clearBlockableActions() {
    lastActionPlayer = nullptr;
    coupTarget = nullptr; // Also clear coup target
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
    // Ensure currentTurn is a valid index before accessing _players
    if (currentTurn >= _players.size()) {
        return nullptr; // Should not happen if logic is sound, but as a safeguard
    }
    return _players[currentTurn];
}