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

Game::Game() : currentTurn(0), gameEnded(false), rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    // Constructor initializes turn index to 0 and game as not ended
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

        clearBlockableActions();
        _players[currentTurn]->onBeginTurn();
        return;
    }

    // If no extra turns, continue normally
    // אפס את התור של כולם (גם מתים)
    for (Player* p : _players) {
        p->setTurn(false);
    }

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
        if (aliveCount == 1) {
            std::cout << "Game ended! Winner: " << winner() << std::endl;
        } else {
            std::cout << "Game ended in a draw - no players remaining!" << std::endl;
        }
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
    
    //std::cout << "Current turn: " << currentTurn << std::endl;
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
    
    // restore the target player from elimination
    if (coupTarget != nullptr) {
        coupTarget->restoreFromElimination();
    }
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
    return _players[currentTurn];
}

// דוגמת שימוש במשחק:
/*
// יצירת משחק חדש
Game game;

// קבלת שמות שחקנים מהממשק הגרפי
std::vector<std::string> playerNames = {"Alice", "Bob", "Charlie", "Diana"};

// איתחול המשחק
try {
    game.initializeGame(playerNames);
    
    // המשחק מוכן להתחיל
    std::cout << "Current turn: " << game.turn() << std::endl;
    
    // הצגת מידע על השחקנים
    auto playersInfo = game.getPlayersWithRoles();
    for (const auto& info : playersInfo) {
        std::cout << info.first << " (" << info.second << ")" << std::endl;
    }
    
    // דוגמה לפלט אפשרי:
    // Player Alice assigned role: Judge
    // Player Bob assigned role: Governor  
    // Player Charlie assigned role: Judge
    // Player Diana assigned role: Merchant
    
} catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
}

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