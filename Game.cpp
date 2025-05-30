#include "Game.hpp"
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <random>
#include <chrono>

// Include role classes
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"

/**
 * @brief Constructs a new Game object.
 * * Initializes the game state including the current turn index, game end status,
 * winner's name, and sets up the random number generator using the current time.
 */
Game::Game() : currentTurn(0), gameEnded(false), _winnerName(""), rng(std::chrono::steady_clock::now().time_since_epoch().count()) {
    // Constructor initializes turn index to 0, game as not ended, and winner name.
    // Initializes random number generator with current time.
}

/**
 * @brief Destroys the Game object.
 * * Cleans up all dynamically allocated Player pointers to prevent memory leaks.
 */
Game::~Game() {
    // Destructor - clean up all player pointers.
    for (Player* player : _players) {
        delete player;
    }
    _players.clear();
}

/**
 * @brief Initializes the game with a list of player names.
 * * Validates the number of players (2-6), shuffles available roles,
 * and creates a new Player object for each name with an assigned role.
 * * @param playerNames A vector of strings, where each string is a player's name.
 * @throws std::invalid_argument if the number of players is not between 2 and 6.
 * @throws std::runtime_error if the game has already been initialized.
 */
void Game::initializeGame(const std::vector<std::string>& playerNames) {
    // Check if the number of players is valid (2-6).
    if (playerNames.size() < 2) {
        throw std::invalid_argument("Game requires at least 2 players");
    }
    if (playerNames.size() > 6) {
        throw std::invalid_argument("Game allows maximum 6 players");
    }
    
    // Check if the game has already started.
    if (!_players.empty()) {
        throw std::runtime_error("Game has already been initialized");
    }
    
    // List of available roles.
    std::vector<std::string> roles = {"Governor", "Baron", "Judge", "Spy", "General", "Merchant"};
    std::shuffle(roles.begin(), roles.end(), rng); // Shuffle the roles.

    // Assign roles to players and add them to the game.
    for (size_t i = 0; i < playerNames.size(); ++i) {
        Player* newPlayer = createPlayerWithRole(playerNames[i], roles[i]);
        _players.push_back(newPlayer);
    }
}

/**
 * @brief Checks if the game can be started.
 * * A game can start if there are between 2 and 6 players initialized.
 * * @return True if the game can start, false otherwise.
 */
bool Game::canStartGame() const {
    return _players.size() >= 2 && _players.size() <= 6;
}

/**
 * @brief Adds a player to the game.
 * * This method is generally used during game setup if players are added individually
 * rather than through `initializeGame`.
 * * @param player A pointer to the Player object to add.
 */
void Game::addPlayer(Player* player) {
    _players.push_back(player);
}

/**
 * @brief Advances the game to the next turn.
 * * Clears the "last arrested" flag for all players, handles extra turns for players
 * who bribed, and then moves to the next alive player in sequence.
 * If only one player remains alive, the game ends.
 * Calls `onBeginTurn` for the new current player.
 */
void Game::nextTurn() {
    // Clear the "last arrested" flag from all players at the start of a new turn sequence.
    clearLastArrestedFlag();

    // Handle extra turns for players who bribed.
    if (extraTurnsRemaining > 0) {
        extraTurnsRemaining--;
        // The current player gets an extra turn, so we don't advance currentTurn.
    } else {
        // Advance to the next living player.
        size_t originalCurrentTurn = currentTurn;
        do {
            currentTurn = (currentTurn + 1) % _players.size();
        } while (!_players[currentTurn]->isAlive() && currentTurn != originalCurrentTurn); // Loop until alive player or back to start.
        
        // If all players are eliminated except one, the game ends.
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

    // Call onBeginTurn for the new current player.
    _players[currentTurn]->onBeginTurn();
}

/**
 * @brief Returns a string indicating whose turn it is.
 * * @return A string representing the current player's turn, or "Game Over"
 * if the game has ended, or an error message if no players are set.
 */
std::string Game::turn() const {
    if (gameEnded) {
        return "Game Over";
    }
    if (_players.empty()) {
        return "No players yet.";
    }
    if (currentTurn >= _players.size()) {
        return "Error: Invalid turn index."; // Should not happen.
    }
    return _players[currentTurn]->getName() + "'s turn.";
}

/**
 * @brief Retrieves a list of all player names in the game.
 * * @return A vector of strings, where each string is a player's name.
 */
std::vector<std::string> Game::players() const {
    std::vector<std::string> playerNames;
    for (const auto& player : _players) {
        playerNames.push_back(player->getName());
    }
    return playerNames;
}

/**
 * @brief Returns the name of the winner if the game has ended.
 * * @return The name of the winning player, or "No winner yet." if the game is still ongoing.
 */
std::string Game::winner() const {
    if (gameEnded) {
        return _winnerName;
    }
    return "No winner yet.";
}

/**
 * @brief Factory method to create a Player object with a specific role.
 * * @param name The name of the player.
 * @param role The role to assign to the player (e.g., "Governor", "Baron").
 * @return A pointer to the newly created Player object.
 * @throws std::invalid_argument if an unknown role is provided.
 */
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

/**
 * @brief Clears the "last arrested" flag for all players.
 * * This is typically called at the beginning of a new turn sequence
 * to reset player states related to arrests.
 */
void Game::clearLastArrestedFlag() {
    for (Player* player : _players) {
        player->gotArrested(false);
    }
}

/**
 * @brief Counts the number of players currently alive in the game.
 * * @return The number of alive players.
 */
size_t Game::getAlivePlayerCount() const {
    size_t count = 0;
    for (const Player* player : _players) {
        if (player->isAlive()) {
            count++;
        }
    }
    return count;
}

/**
 * @brief Gets a pointer to the current player whose turn it is.
 * * @return A pointer to the current Player object, or nullptr if no players exist
 * or the game has ended.
 */
Player* Game::getCurrentPlayer() const {
    if (_players.empty() || gameEnded) {
        return nullptr;
    }
    
    // Find the next alive player if currentTurn is on an eliminated player.
    // This assumes nextTurn() already handles advancing to an alive player,
    // but this is a safeguard for direct access.
    size_t tempTurn = currentTurn;
    while (!_players[tempTurn]->isAlive()) {
        tempTurn = (tempTurn + 1) % _players.size();
        if (tempTurn == currentTurn) { // Looped through all and none are alive (should be handled by gameEnded).
            return nullptr;
        }
    }
    return _players[tempTurn];
}

/**
 * @brief Grants a specified number of extra turns to the current player.
 * * These extra turns will be processed before the normal turn sequence resumes.
 * * @param turns The number of extra turns to grant.
 */
void Game::giveExtraTurns(int turns) {
    extraTurnsRemaining += turns;
}

/**
 * @brief Retrieves a list of all players and their assigned roles.
 * * @return A vector of pairs, where each pair contains a player's name (string)
 * and their role (string).
 */
std::vector<std::pair<std::string, std::string>> Game::getPlayersWithRoles() const {
    std::vector<std::pair<std::string, std::string>> playerRoles;
    for (const Player* player : _players) {
        playerRoles.push_back({player->getName(), player->role()});
    }
    return playerRoles;
}

/**
 * @brief Records the last action performed in the game.
 * * This information can be used by other game mechanics, such as blocking actions.
 * * @param performer A pointer to the Player who performed the action.
 * @param actionType A string describing the type of action (e.g., "bribe", "tax", "coup").
 * @param target A pointer to the Player who was the target of the action, or nullptr if no target.
 */
void Game::recordAction(Player* performer, const std::string& actionType, Player* target) {
    _lastActionPerformer = performer;
    _lastActionType = actionType;
    _lastActionTarget = target;
}

/**
 * @brief Clears the record of the last action.
 * * Resets the last action performer, type, target, and any associated tax amount.
 */
void Game::clearLastAction() {
    _lastActionPerformer = nullptr;
    _lastActionType = "";
    _lastActionTarget = nullptr;
    lastTaxAmount = 0; // Clear stored tax amount.
}

/**
 * @brief Attempts to find a player who can block a given action.
 * * Iterates through all alive players (excluding the performer) to see if
 * any of them have the ability to block the specified action type.
 * * @param actionType The type of action to block (e.g., "bribe", "tax", "coup").
 * @param performer A pointer to the Player who performed the action.
 * @param target A pointer to the Player who was the target of the action (can be nullptr).
 * @return A pointer to the Player who can block the action, or nullptr if no one can.
 */
Player* Game::tryBlock(const std::string& actionType, Player* performer, Player* target) {
    for (Player* p : _players) {
        // A player cannot block their own action.
        if (p->isAlive() && p != performer) { 
            if (actionType == "bribe") {
                // Judge can block bribe.
                if (p->canUndoBribe()) { 
                    return p;
                }
            } else if (actionType == "tax") {
                // Governor can block tax.
                if (p->canBlockTax()) { 
                    return p;
                }
            } else if (actionType == "coup") {
                // General can block Coup, but only if they have enough coins (5 coins).
                if (p->canBlockCoup() && p->getCoins() >= 5) { 
                    return p;
                }
            }
            
        }
    }
    return nullptr; // No one can block this action.
}