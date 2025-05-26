#include <vector>
#include <string>
#include "Player.hpp"

class Game {
    std::vector<Player*> players;
    size_t currentTurn;
    bool gameEnded;
public:
    Game();
    ~Game();

    void addPlayer(Player* player);
    void nextTurn();
    std::string turn() const;
    std::vector<std::string> activePlayers() const;
    std::string winner() const;
};
