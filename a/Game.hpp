#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include "Player.hpp"

class Game {
private:
    std::vector<Player*> _players;
    size_t currentTurn;
    bool gameEnded;

    // Blocking system - used to manage actions that can be blocked or undone
    Player* lastActionPlayer = nullptr;
    Player* coupTarget = nullptr; // for blocking coup
    bool canBlockBribe = false;
    bool canBlockTax = false;
    bool canBlockCoup = false;

    // Extra turns system for bribe
    int extraTurnsRemaining = 0; 

public:
    // Constructor and Destructor
    Game();
    ~Game();

    // Core game methods
    void addPlayer(Player* player);
    void nextTurn();
    std::string turn() const;
    std::vector<std::string> players() const;
    std::string winner() const;

    // Additional utility methods
    bool isGameEnded() const { return gameEnded; }
    size_t getPlayerCount() const { return _players.size(); }
    size_t getAlivePlayerCount() const;
    
    // Get current player (for game logic)
    Player* getCurrentPlayer() const;
    
    // Blocking methods
    void recordBribe(Player* player);
    void recordTax(Player* player);
    void recordCoup(Player* player, Player* target);
    
    bool tryBlockBribe(Player* blocker);
    bool tryBlockTax(Player* blocker);
    bool tryBlockCoup(Player* blocker);
    
    void clearBlockableActions(); // Called in nextTurn()
    
    // Method to clear last arrested flag from all players
    void clearLastArrestedFlag();

    // Extra turns methods
    void giveExtraTurns(int turns = 2); // gives extra turns to the player who bribed
    bool hasExtraTurns() const { return extraTurnsRemaining > 0; }
    int getExtraTurnsRemaining() const { return extraTurnsRemaining; }
    
    // Prevent copy constructor and assignment operator
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
};

#endif // GAME_HPP