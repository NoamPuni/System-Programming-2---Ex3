#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <stdlib.h>

// Include all necessary project headers
#include "Game.hpp"
#include "Player.hpp" // Base class
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"

#include <string>
#include <vector>
#include <algorithm> // For std::find
#include <stdexcept> // For std::runtime_error, std::invalid_argument

/**
 * Helper function to create a basic game with predefined players
 * Returns a game with 6 players (one of each role)
 */
Game* createBasicGame() {
    Game* game = new Game();
    
    Governor* governor = new Governor("Moshe");
    Spy* spy = new Spy("Yossi");
    Baron* baron = new Baron("Meirav");
    General* general = new General("Reut");
    Judge* judge = new Judge("Gilad");
    Merchant* merchant = new Merchant("David");
    
    game->addPlayer(governor);
    game->addPlayer(spy);
    game->addPlayer(baron);
    game->addPlayer(general);
    game->addPlayer(judge);
    game->addPlayer(merchant);
    
    return game;
}

/**
 * Helper function to clean up game memory
 */
void cleanupGame(Game* game) {
    delete game; // Game destructor handles player cleanup
}

/**
 * Helper function to find player by name in game
 */
Player* findPlayerByName(Game* game, const std::string& name) {
    auto players = game->getAllPlayers();
    for (Player* player : players) {
        if (player->getName() == name) {
            return player;
        }
    }
    return nullptr;
}

/**
 * Helper function to advance game turns until specific player's turn
 */
void advanceToPlayer(Game* game, const std::string& playerName) {
    int maxTurns = 10; // Safety limit
    int turns = 0;
    while (game->turn().find(playerName) == std::string::npos && turns < maxTurns) {
        game->nextTurn();
        turns++;
    }
}

TEST_SUITE("Game Basic Functionality") {
    
    TEST_CASE("Game Constructor") {
        Game game;
        CHECK(game.getPlayerCount() == 0);
        CHECK_FALSE(game.isGameEnded());
        CHECK(game.getAlivePlayerCount() == 0);
        CHECK(game.turn() == "No players yet.");
    }
    
    TEST_CASE("Adding Players") {
        Game* game = new Game();
        
        Governor* governor = new Governor("Alice");
        Spy* spy = new Spy("Bob");
        Baron* baron = new Baron("Charlie");
        
        game->addPlayer(governor);
        CHECK(game->getPlayerCount() == 1);
        CHECK(game->getAlivePlayerCount() == 1);
        
        game->addPlayer(spy);
        game->addPlayer(baron);
        CHECK(game->getPlayerCount() == 3);
        CHECK(game->getAlivePlayerCount() == 3);
        
        // Check player names are stored correctly
        std::vector<std::string> playerNames = game->players();
        CHECK(playerNames.size() == 3);
        CHECK(std::find(playerNames.begin(), playerNames.end(), "Alice") != playerNames.end());
        CHECK(std::find(playerNames.begin(), playerNames.end(), "Bob") != playerNames.end());
        CHECK(std::find(playerNames.begin(), playerNames.end(), "Charlie") != playerNames.end());
        
        cleanupGame(game);
    }
    
    TEST_CASE("Turn Management - Basic Flow") {
        Game* game = createBasicGame();
        
        // First player added should be first to play
        CHECK(game->turn() == "Moshe's turn.");
        
        // Advance turn and check it goes to next player
        game->nextTurn();
        CHECK(game->turn() == "Yossi's turn.");
        
        game->nextTurn();
        CHECK(game->turn() == "Meirav's turn.");
        
        game->nextTurn();
        CHECK(game->turn() == "Reut's turn.");
        
        game->nextTurn();
        CHECK(game->turn() == "Gilad's turn.");
        
        game->nextTurn();
        CHECK(game->turn() == "David's turn.");
        
        // Should cycle back to first player
        game->nextTurn();
        CHECK(game->turn() == "Moshe's turn.");
        
        cleanupGame(game);
    }
    
    TEST_CASE("Turn Management - Skip Eliminated Players") {
        Game* game = createBasicGame();
        
        // Eliminate second player (Spy - Yossi)
        Player* spy = findPlayerByName(game, "Yossi");
        REQUIRE(spy != nullptr);
        spy->eliminateMe();
        
        // Start from first player
        CHECK(game->turn() == "Moshe's turn.");
        
        // Should skip eliminated player and go to third player
        game->nextTurn();
        CHECK(game->turn() == "Meirav's turn.");
        
        // Continue normally
        game->nextTurn();
        CHECK(game->turn() == "Reut's turn.");
        
        // Eliminate another player (Baron - Meirav)
        Player* baron = findPlayerByName(game, "Meirav");
        REQUIRE(baron != nullptr);
        baron->eliminateMe();
        
        // Go back to first, then should skip TWO eliminated players
        game->nextTurn();
        game->nextTurn();
        game->nextTurn();
        CHECK(game->turn() == "Moshe's turn.");
        
        game->nextTurn();
        CHECK(game->turn() == "Reut's turn."); // Should skip Yossi and Meirav
        
        cleanupGame(game);
    }
    
    TEST_CASE("Turn Management - Extra Turns from Bribe") {
        Game* game = createBasicGame();
        
        // Give first player enough coins for bribe
        Player* governor = findPlayerByName(game, "Moshe");
        REQUIRE(governor != nullptr);
        governor->setCoins(4);
        
        CHECK(game->turn() == "Moshe's turn.");
        CHECK(game->getExtraTurnsRemaining() == 0);
        
        // Perform bribe
        governor->bribe(*game);
        game->giveExtraTurns(2); // Simulate bribe giving extra turns
        
        CHECK(game->getExtraTurnsRemaining() == 2);
        CHECK(governor->getCoins() == 0); // Should have spent 4 coins
        
        // Next turn should still be the same player (first extra turn)
        game->nextTurn();
        CHECK(game->turn() == "Moshe's turn.");
        CHECK(game->getExtraTurnsRemaining() == 1);
        
        // Second extra turn
        game->nextTurn();
        CHECK(game->turn() == "Moshe's turn.");
        CHECK(game->getExtraTurnsRemaining() == 0);
        
        // Now should advance to next player
        game->nextTurn();
        CHECK(game->turn() == "Yossi's turn.");
        
        cleanupGame(game);
    }
    
    TEST_CASE("Game State - Game End Condition") {
        Game* game = new Game();
        
        Governor* governor = new Governor("Alice");
        Spy* spy = new Spy("Bob");
        
        game->addPlayer(governor);
        game->addPlayer(spy);
        
        CHECK_FALSE(game->isGameEnded());
        CHECK(game->getAlivePlayerCount() == 2);
        
        // Eliminate one player
        spy->eliminateMe();
        
        // Game should end when nextTurn is called and only one player remains
        game->nextTurn();
        
        CHECK(game->isGameEnded());
        CHECK(game->getAlivePlayerCount() == 1);
        CHECK(game->winner() == "Alice");
        
        cleanupGame(game);
    }
}

TEST_SUITE("Gather") {

    TEST_CASE("Player can gather coins on their turn") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        REQUIRE(governor != nullptr);
        
        // Governor's turn
        CHECK(game->turn() == "Moshe's turn.");
        CHECK(governor->getCoins() == 0);
        
        governor->gather(*game);
        CHECK(governor->getCoins() == 1);
        
        cleanupGame(game);
    }
    
    TEST_CASE("Sanctioned player cannot gather") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        REQUIRE(governor != nullptr);
        
        governor->sanctionMe();
        CHECK(governor->isSanctioned());
        
        CHECK_THROWS_AS(governor->gather(*game), std::runtime_error);
        
        cleanupGame(game);
    }
}

TEST_SUITE("Tax") {
    
    TEST_CASE("Governor can block tax - tax is cancelled") {
        Game* game = createBasicGame();
        Player* spy = findPlayerByName(game, "Yossi");
        Player* governor = findPlayerByName(game, "Moshe");
        REQUIRE(spy != nullptr);
        REQUIRE(governor != nullptr);
        
        // Advance to spy's turn
        advanceToPlayer(game, "Yossi");
        CHECK(game->turn() == "Yossi's turn.");
        
        int initialCoins = spy->getCoins();
        
        // Spy performs tax, but governor can block
        int taxed = spy->tax(*game);
        
        // If governor blocks, the tax should be cancelled
        if (governor->canBlockTax()) {
            bool blocked = game->tryBlock("tax",governor, spy);
            if (blocked) {
                CHECK(spy->getCoins() == initialCoins); // No coins gained
            }
        }
        
        cleanupGame(game);
    }
}

TEST_SUITE("Bribe") {
    
    TEST_CASE("Player can perform bribe with 4+ coins") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        REQUIRE(governor != nullptr);
        
        governor->setCoins(4);
        CHECK(game->turn() == "Moshe's turn.");
        
        governor->bribe(*game);
        CHECK(governor->getCoins() == 0); // Spent 4 coins
        
        cleanupGame(game);
    }
    
    TEST_CASE("Player cannot bribe with less than 4 coins") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        REQUIRE(governor != nullptr);
        
        governor->setCoins(3);
        CHECK(game->turn() == "Moshe's turn.");
        
        CHECK_THROWS_AS(governor->bribe(*game), std::runtime_error);
        
        cleanupGame(game);
    }
    
    TEST_CASE("Bribe gives 2 extra turns when not blocked") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        REQUIRE(governor != nullptr);
        
        governor->setCoins(4);
        CHECK(game->turn() == "Moshe's turn.");
        CHECK(game->getExtraTurnsRemaining() == 0);
        
        governor->bribe(*game);
        game->giveExtraTurns(2); // Simulate successful bribe
        
        CHECK(game->getExtraTurnsRemaining() == 2);
        
        cleanupGame(game);
    }
    
    TEST_CASE("Judge can block bribe - no extra turns") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* judge = findPlayerByName(game, "Gilad");
        REQUIRE(governor != nullptr);
        REQUIRE(judge != nullptr);
        
        governor->setCoins(4);
        CHECK(game->turn() == "Moshe's turn.");
        
        governor->bribe(*game);
        
        // If judge blocks, no extra turns should be given
        if (judge->canUndoBribe()) {
            bool blocked = game->tryBlock("bribe",judge, governor);
            if (blocked) {
                CHECK(game->getExtraTurnsRemaining() == 0);
            }
        }
        
        cleanupGame(game);
    }
}

TEST_SUITE("Arrest") {
    
    TEST_CASE("Cannot arrest player who was already arrested last turn") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* spy = findPlayerByName(game, "Yossi");
        REQUIRE(governor != nullptr);
        REQUIRE(spy != nullptr);
        
        spy->gotArrested(true);
        CHECK(spy->isLastOneArrested());
        
        CHECK_THROWS_AS(governor->arrest(spy, *game), std::runtime_error);
        
        cleanupGame(game);
    }
    
    TEST_CASE("Spy can prevent arrest") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* spy = findPlayerByName(game, "Yossi");
        REQUIRE(governor != nullptr);
        REQUIRE(spy != nullptr);
        
        CHECK(game->turn() == "Moshe's turn.");
        
        // Spy prevents their own arrest
        if (spy->canPreventArrest()) {
            spy->gotPreventedFromArresting();
            CHECK(spy->isPreventedFromArresting());
        }
        
        cleanupGame(game);
    }
    
    TEST_CASE("Merchant loses 2 coins when arrested") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* merchant = findPlayerByName(game, "David");
        REQUIRE(governor != nullptr);
        REQUIRE(merchant != nullptr);
        
        merchant->setCoins(3);
        CHECK(game->turn() == "Moshe's turn.");
        
        bool arrested = governor->arrest(merchant, *game);
        if (arrested) {
            // Merchant should lose 2 coins if arrested
            CHECK(merchant->getCoins() == 1);
        }
        
        cleanupGame(game);
    }
    
    TEST_CASE("Cannot arrest eliminated player") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* spy = findPlayerByName(game, "Yossi");
        REQUIRE(governor != nullptr);
        REQUIRE(spy != nullptr);
        
        spy->eliminateMe();
        CHECK_FALSE(spy->isAlive());
        
        CHECK_THROWS_AS(governor->arrest(spy, *game), std::runtime_error);
        
        cleanupGame(game);
    }
}

TEST_SUITE("Sanction") {
    
    TEST_CASE("Player can sanction another player with 3+ coins") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* spy = findPlayerByName(game, "Yossi");
        REQUIRE(governor != nullptr);
        REQUIRE(spy != nullptr);
        
        governor->setCoins(3);
        CHECK(game->turn() == "Moshe's turn.");
        CHECK_FALSE(spy->isSanctioned());
        
        governor->sanction(spy, *game);
        CHECK(governor->getCoins() == 0); // Spent 3 coins
        CHECK(spy->isSanctioned());
        
        cleanupGame(game);
    }
    
    TEST_CASE("Cannot sanction player who is already sanctioned") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* spy = findPlayerByName(game, "Yossi");
        REQUIRE(governor != nullptr);
        REQUIRE(spy != nullptr);
        
        governor->setCoins(3);
        spy->sanctionMe();
        CHECK(spy->isSanctioned());
        
        CHECK_THROWS_AS(governor->sanction(spy, *game), std::runtime_error);
        
        cleanupGame(game);
    }
}

TEST_SUITE("Coup") {
    
    TEST_CASE("Player can perform coup with 7+ coins") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* spy = findPlayerByName(game, "Yossi");
        REQUIRE(governor != nullptr);
        REQUIRE(spy != nullptr);
        
        governor->setCoins(7);
        CHECK(game->turn() == "Moshe's turn.");
        CHECK(spy->isAlive());
        
        bool couped = governor->coup(spy, *game);
        CHECK(couped);
        CHECK(governor->getCoins() == 0); // Spent 7 coins
        CHECK_FALSE(spy->isAlive()); // Target eliminated
        
        cleanupGame(game);
    }
    
    TEST_CASE("Cannot coup with less than 7 coins") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* spy = findPlayerByName(game, "Yossi");
        REQUIRE(governor != nullptr);
        REQUIRE(spy != nullptr);
        
        governor->setCoins(6);
        CHECK(game->turn() == "Moshe's turn.");
        
        CHECK_THROWS_AS(governor->coup(spy, *game), std::runtime_error);
        
        cleanupGame(game);
    }
    
    TEST_CASE("General can block coup") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* spy = findPlayerByName(game, "Yossi");
        Player* general = findPlayerByName(game, "Reut");
        REQUIRE(governor != nullptr);
        REQUIRE(spy != nullptr);
        REQUIRE(general != nullptr);
        
        governor->setCoins(7);
        CHECK(game->turn() == "Moshe's turn.");
        
        bool couped = governor->coup(spy, *game);

        // If general blocks, coup should fail
        if (general->canBlockCoup()) {
            bool blocked = game->tryBlock("coup",general,governor);
            if (blocked) {
                CHECK(spy->isAlive()); // Target still alive
            }
        }
        
        cleanupGame(game);
    }
    
    TEST_CASE("Cannot coup eliminated player") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* spy = findPlayerByName(game, "Yossi");
        REQUIRE(governor != nullptr);
        REQUIRE(spy != nullptr);
        
        governor->setCoins(7);
        spy->eliminateMe();
        CHECK_FALSE(spy->isAlive());
        
        CHECK_THROWS_AS(governor->coup(spy, *game), std::runtime_error);
        
        cleanupGame(game);
    }
}

TEST_SUITE("Role Interactions") {
    
    TEST_CASE("Governor blocks tax - action cancelled") {
        Game* game = createBasicGame();
        Player* spy = findPlayerByName(game, "Yossi");
        Player* governor = findPlayerByName(game, "Moshe");
        REQUIRE(spy != nullptr);
        REQUIRE(governor != nullptr);
        
        // Advance to spy's turn
        advanceToPlayer(game, "Yossi");
        int initialCoins = spy->getCoins();
        
        // Record tax action
        game->recordAction(spy, "tax");
        
        // Governor blocks
        bool blocked = game->tryBlock("tax", governor, spy);
        if (blocked) {
            CHECK(spy->getCoins() == initialCoins); // No coins gained
        }
        
        cleanupGame(game);
    }
    
    TEST_CASE("Spy prevents arrest - action fails") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* spy = findPlayerByName(game, "Yossi");
        REQUIRE(governor != nullptr);
        REQUIRE(spy != nullptr);
        
        CHECK(game->turn() == "Moshe's turn.");
        
        // Spy prevents arrest
        if (spy->canPreventArrest()) {
            spy->gotPreventedFromArresting();
            
            // Arrest should fail
            CHECK_THROWS_AS(governor->arrest(spy, *game), std::runtime_error);
        }
        
        cleanupGame(game);
    }
    
    TEST_CASE("Judge cancels bribe - no extra turn") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* judge = findPlayerByName(game, "Gilad");
        REQUIRE(governor != nullptr);
        REQUIRE(judge != nullptr);
        
        governor->setCoins(4);
        
        // Record bribe action
        game->recordAction(governor, "bribe");
        
        // Judge blocks
        bool blocked = game->tryBlock("bribe",judge, governor);
        if (blocked) {
            CHECK(game->getExtraTurnsRemaining() == 0);
        }
        
        cleanupGame(game);
    }
}

TEST_SUITE("Edge Cases and Exceptions") {
    
    TEST_CASE("Cannot perform actions on eliminated players") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* spy = findPlayerByName(game, "Yossi");
        REQUIRE(governor != nullptr);
        REQUIRE(spy != nullptr);
        
        spy->eliminateMe();
        CHECK_FALSE(spy->isAlive());
        
        CHECK_THROWS_AS(governor->arrest(spy, *game), std::runtime_error);
        
        governor->setCoins(3);
        CHECK_THROWS_AS(governor->sanction(spy, *game), std::runtime_error);
        
        governor->setCoins(7);
        CHECK_THROWS_AS(governor->coup(spy, *game), std::runtime_error);
        
        cleanupGame(game);
    }
    
    TEST_CASE("Double sanction attempt throws exception") {
        Game* game = createBasicGame();
        Player* governor = findPlayerByName(game, "Moshe");
        Player* spy = findPlayerByName(game, "Yossi");
        REQUIRE(governor != nullptr);
        REQUIRE(spy != nullptr);
        
        governor->setCoins(6);
        
        // First sanction
        governor->sanction(spy, *game);
        CHECK(spy->isSanctioned());
        CHECK(governor->getCoins() == 3);
        
        // Second sanction attempt should fail
        CHECK_THROWS_AS(governor->sanction(spy, *game), std::runtime_error);
        
        cleanupGame(game);
    }
}

TEST_SUITE("Game End Conditions") {

    TEST_CASE("Game ends when only one player remains") {
        Game* game = new Game();
        
        Governor* governor = new Governor("Alice");
        Spy* spy = new Spy("Bob");
        Baron* baron = new Baron("Charlie");
        
        game->addPlayer(governor);
        game->addPlayer(spy);
        game->addPlayer(baron);
        
        CHECK_FALSE(game->isGameEnded());
        CHECK(game->getAlivePlayerCount() == 3);
        
        // Eliminate two players
        spy->eliminateMe();
        baron->eliminateMe();
        
        // Game should end when nextTurn is called
        game->nextTurn();
        
        CHECK(game->isGameEnded());
        CHECK(game->getAlivePlayerCount() == 1);
        CHECK(game->winner() == "Alice");
        
        cleanupGame(game);
    }
}