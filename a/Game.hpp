#ifndef GAME_HPP
#define GAME_HPP

#include <vector>
#include <string>
#include <random>
#include "Player.hpp"

class Game {
private:
    std::vector<Player*> _players; // Stores all players in the game.
    size_t currentTurn; // Index of the current player's turn.
    bool gameEnded; // Flag indicating if the game has ended.
    std::string _winnerName; // Name of the winning player.
    
    int extraTurnsRemaining = 0; // Number of extra turns remaining for the current player.

    mutable std::mt19937 rng; // Random number generator for game mechanics.

    Player* createPlayerWithRole(const std::string& name, const std::string& role); // Helper to create a player with a specific role.

public:
    Game(); // Constructor for the Game class.
    ~Game(); // Destructor for the Game class.

    void initializeGame(const std::vector<std::string>& playerNames); // Initializes the game with player names and roles.
    bool canStartGame() const; // Checks if the game has enough players to start.

    void addPlayer(Player* player); // Adds a player to the game.
    void nextTurn(); // Advances the game to the next turn.
    std::string turn() const; // Returns a string indicating whose turn it is.
    std::vector<std::string> players() const; // Returns a list of all player names.
    std::string winner() const; // Returns the name of the game winner.

    bool isGameEnded() const { return gameEnded; } // Checks if the game has concluded.
    size_t getPlayerCount() const { return _players.size(); } // Returns the total number of players.
    size_t getAlivePlayerCount() const; // Returns the count of players who are still alive.
    
    Player* getCurrentPlayer() const; // Returns a pointer to the current player.
    
    void recordBribe(Player* player); // Records a bribe action.
    void recordTax(Player* player, int amount); // Records a tax action with the amount.
    void recordCoup(Player* player, Player* target); // Records a coup action.
    
    bool tryBlockBribe(Player* blocker); // Attempts to block a bribe action.
    bool tryBlockTax(Player* blocker); // Attempts to block a tax action.
    bool tryBlockCoup(Player* blocker); // Attempts to block a coup action.
    
    void clearLastArrestedFlag(); // Clears the 'last arrested' flag for all players.

    void giveExtraTurns(int turns = 2); // Grants extra turns to the current player.
    bool hasExtraTurns() const { return extraTurnsRemaining > 0; } // Checks if extra turns are remaining.
    int getExtraTurnsRemaining() const { return extraTurnsRemaining; } // Returns the number of remaining extra turns.

    Player* _lastActionPerformer = nullptr; // Stores the player who performed the last action.
    std::string _lastActionType = ""; // Stores the type of the last action.
    Player* _lastActionTarget = nullptr; // Stores the target of the last action.
    int lastTaxAmount = 0; // Stores the amount of the last tax action.
    void setLastTaxAmount(int amount) { lastTaxAmount = amount; } // Sets the amount of the last tax action.

    void recordAction(Player* performer, const std::string& actionType, Player* target = nullptr); // Records general action details.
    void clearLastAction(); // Clears the record of the last action.
    Player* tryBlock(const std::string& actionType, Player* performer, Player* target); // Attempts to find a player who can block a given action.
    int getLastTaxAmount() const { return lastTaxAmount; } // Returns the amount of the last tax action.
    
    std::vector<std::pair<std::string, std::string>> getPlayersWithRoles() const; // Returns a list of players with their roles.
    
    Game(const Game&) = delete; // Prevents copying the Game object.
    Game& operator=(const Game&) = delete; // Prevents assigning the Game object.

    const std::vector<Player*> getAllPlayers() const { return _players; } // Returns all players in the game.

};

#endif // GAME_HPP