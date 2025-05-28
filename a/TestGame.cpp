#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

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

TEST_CASE("Basic Tests – General Game Logic") {

    SUBCASE("Game and Player Creation") {
        INFO("Checks game creation, player addition, and initial checks");
        Game game; // Create a new game instance

        REQUIRE_FALSE(game.isGameEnded()); //
        REQUIRE(game.getPlayerCount() == 0); //

        // Create players with their actual roles
        Player* p1 = new Governor("Alice"); //
        Player* p2 = new Spy("Bob"); //
        // Player* p3 = new Baron("Charlie"); // Will create later if needed

        game.addPlayer(p1); //
        CHECK(game.getPlayerCount() == 1);
        game.addPlayer(p2); //
        CHECK(game.getPlayerCount() == 2);

        // Check that player names match the input
        std::vector<std::string> current_players_names = game.players(); //
        CHECK(current_players_names.size() == 2);
        CHECK(std::find(current_players_names.begin(), current_players_names.end(), "Alice") != current_players_names.end());
        CHECK(std::find(current_players_names.begin(), current_players_names.end(), "Bob") != current_players_names.end());

        // Check that players start with 0 coins and are not eliminated (alive)
        CHECK(p1->getCoins() == 0); //
        CHECK(p1->isAlive()); //
        CHECK(p2->getCoins() == 0); //
        CHECK(p2->isAlive()); //

        // Attempt to add a null player
        Player* null_player = nullptr;
        CHECK_THROWS_AS(game.addPlayer(null_player), std::invalid_argument); //

        // Attempt to add players after the game has ended
        // To end the game, leave only one player "alive" and call nextTurn
        Game game_for_end_test;
        Player* player_to_survive = new General("Survivor"); //
        Player* player_to_be_eliminated = new Merchant("Victim"); //

        game_for_end_test.addPlayer(player_to_survive);
        game_for_end_test.addPlayer(player_to_be_eliminated);

        REQUIRE(game_for_end_test.getPlayerCount() == 2);
        player_to_be_eliminated->eliminateMe(); //
        REQUIRE_FALSE(player_to_be_eliminated->isAlive());

        // Calling nextTurn should detect that only one player remains and end the game
        game_for_end_test.nextTurn(); //
                                     // This should set gameEnded to true because only one player is alive.

        REQUIRE(game_for_end_test.isGameEnded()); //

        Player* p_late = new Judge("Latecomer"); //
        CHECK_THROWS_AS(game_for_end_test.addPlayer(p_late), std::runtime_error); //
        delete p_late; // Because it won't be added to the game and thus not cleaned up by it
    }

    SUBCASE("Turn Management") {
        INFO("Checks the logic of turn progression");

        Game turn_game;
        Player* player_A = new Baron("PlayerA"); //
        Player* player_B = new Judge("PlayerB"); //
        Player* player_C = new General("PlayerC"); //

        turn_game.addPlayer(player_A);
        turn_game.addPlayer(player_B);
        turn_game.addPlayer(player_C);

        // The first player added is the first in turn
        CHECK(turn_game.turn() == "PlayerA"); //

        // After calling nextTurn, the turn passes to the next player
        turn_game.nextTurn(); //
        CHECK(turn_game.turn() == "PlayerB"); //

        turn_game.nextTurn(); //
        CHECK(turn_game.turn() == "PlayerC"); //

        turn_game.nextTurn(); // Back to the first
        CHECK(turn_game.turn() == "PlayerA"); //

        // If a player is eliminated – the turn automatically passes to the next non-eliminated player
        // player_A is in turn. Eliminate player_B.
        player_B->eliminateMe(); //
        REQUIRE_FALSE(player_B->isAlive());

        // Now it's player_A's turn. After him, it should be player_C (because B was eliminated).
        turn_game.nextTurn(); // player_A finishes turn
        CHECK(turn_game.turn() == "PlayerC"); // Turn should pass to player_C

        // Eliminate player_A as well. Now player_C is in turn.
        player_A->eliminateMe(); //
        REQUIRE_FALSE(player_A->isAlive());

        // player_C is the only one remaining. The turn stays with him.
        turn_game.nextTurn(); // player_C finishes turn.
                                     // nextTurn will detect that only one player remains and end the game.
        CHECK(turn_game.turn() == "PlayerC"); //

        REQUIRE(turn_game.isGameEnded()); //
        REQUIRE(turn_game.players().size() == 1); //
        CHECK(turn_game.winner() == "PlayerC"); //


        // Test case for a player receiving an extra turn
        Game extra_turn_game;
        Player* et_p1 = new Governor("ET_P1"); //
        Player* et_p2 = new Spy("ET_P2"); //
        extra_turn_game.addPlayer(et_p1);
        extra_turn_game.addPlayer(et_p2);

        REQUIRE(extra_turn_game.turn() == "ET_P1"); //

        // Give ET_P1 one extra turn (bribery according to description gives 2, we will test with 1)
        extra_turn_game.giveExtraTurns(1); //
        CHECK(extra_turn_game.hasExtraTurns()); //
        CHECK(extra_turn_game.getExtraTurnsRemaining() == 1); //

        extra_turn_game.nextTurn(); // ET_P1 should continue turn due to the extra turn
        CHECK(extra_turn_game.turn() == "ET_P1"); //
        CHECK_FALSE(extra_turn_game.hasExtraTurns()); // Extra turn was used
        CHECK(extra_turn_game.getExtraTurnsRemaining() == 0); //

        extra_turn_game.nextTurn(); // Now the turn should pass to ET_P2
        CHECK(extra_turn_game.turn() == "ET_P2"); //
    }
}
// Helper function to create players for tests within the Game class.
// This function is for initial setup. In-game actions will be called on Game's current player.
Game createGameWithSpecificPlayers(const vector<pair<string, string>>& playerInfo) {
    Game game;
    for (const auto& info : playerInfo) {
        if (info.second == "Governor") {
            game.addPlayer(new Governor(info.first));
        } else if (info.second == "Judge") {
            game.addPlayer(new Judge(info.first));
        } else if (info.second == "Baron") {
            game.addPlayer(new Baron(info.first));
        } else if (info.second == "General") {
            game.addPlayer(new General(info.first));
        } else if (info.second == "Merchant") {
            game.addPlayer(new Merchant(info.first));
        } else if (info.second == "Spy") {
            game.addPlayer(new Spy(info.first));
        } else {
            // This case should ideally not be hit if all roles are covered, or should throw an error.
            // For now, let's assume all roles are specific.
            throw runtime_error("Unknown player role provided for test setup: " + info.second);
        }
    }
    return game;
}

TEST_SUITE("Basic Actions") {

    TEST_CASE("Gather action") {
        Game game;
        auto p1 = new Governor("Alice");
        auto p2 = new Judge("Bob");
        game.addPlayer(p1);
        game.addPlayer(p2);

        // Player can gather on their turn
        CHECK_EQ(game.turn(), "Alice");
        p1->gather();
        CHECK_EQ(p1->getCoins(), 1); // 1 coin gained

        // Player cannot gather if sanctioned
        p2->sanction(static_cast<Player*>(p1)); // Sanction Alice
        CHECK(p1->isSanctioned());
        CHECK_EQ(game.turn(), "Alice"); // It's still Alice's turn in this test flow
        p1->gather(); // Alice is sanctioned, so gather should not add coins
        CHECK_EQ(p1->getCoins(), 1); // Coins should not increase

        // Player cannot gather out of turn
        CHECK_THROWS_AS(p2->gather(), std::runtime_error); // Bob tries to gather out of turn

        game.nextTurn(); // Alice's sanction should be cleared at the start of her next turn
        CHECK_EQ(game.turn(), "Bob");
        CHECK_FALSE(p1->isSanctioned()); // Sanction cleared at the start of Bob's turn, not Alice's
    }

    TEST_CASE("Tax action") {
        Game game;
        auto p1 = new Player("Alice"); // Using generic Player for base tax check
        auto p2 = new Governor("Bob"); // Governor for tax blocking
        auto p3 = new Judge("Charlie");
        game.addPlayer(p1);
        game.addPlayer(p2);
        game.addPlayer(p3);

        // Player can perform tax on their turn
        CHECK_EQ(game.turn(), "Alice");
        p1->setTurn(true); // Manually set turn for direct player action in test
        p1->tax(game);
        CHECK_EQ(p1->getCoins(), 2); // 2 coins gained
        p1->setTurn(false);

        // Player cannot perform tax if sanctioned
        p3->sanction(std::shared_ptr<Player>(p1)); // Sanction Alice
        CHECK(p1->isSanctioned());
        CHECK_EQ(game.turn(), "Alice"); // Still Alice's turn in this test
        p1->setTurn(true);
        p1->tax(game); // Alice is sanctioned, tax should not add coins
        CHECK_EQ(p1->getCoins(), 2); // Coins should not increase
        p1->setTurn(false);

        // Player cannot perform tax out of turn
        CHECK_THROWS_AS(p3->tax(game), std::runtime_error); // Charlie tries to tax out of turn

        // Governor can block tax action
        game.nextTurn(); // Bob's turn
        CHECK_EQ(game.turn(), "Bob");
        p2->setTurn(true); // Manually set turn for direct player action in test
        p2->tax(game); // Bob taxes (as Governor, gets 3 coins)
        CHECK_EQ(p2->getCoins(), 3);
        p2->setTurn(false);

        game.nextTurn(); // Charlie's turn
        CHECK_EQ(game.turn(), "Charlie");
        p3->setTurn(true);
        p3->tax(game); // Charlie (Judge) taxes, should get 2 coins
        CHECK_EQ(p3->getCoins(), 2); // Should be 2 from tax
        p3->setTurn(false);

        game.nextTurn(); // Back to Alice's turn
        p1->setTurn(true);
        p1->tax(game); // Alice taxes
        CHECK_EQ(p1->getCoins(), 4); // Alice's coins are now 2 (from previous tax) + 2 (from current tax)
        p1->setTurn(false);

        // Now test blocking tax:
        // Alice taxes, Bob (Governor) blocks.
        game.nextTurn(); // Bob's turn
        game.nextTurn(); // Charlie's turn
        game.nextTurn(); // Alice's turn
        
        p1->setTurn(true);
        p1->tax(game); // Alice taxes, now has 6 coins
        CHECK_EQ(p1->getCoins(), 6);
        p1->setTurn(false);

        // It's still Alice's "action" turn if we're considering immediate blocks.
        // In this implementation, the block happens before the next turn if available.
        // We need to simulate the timing of the block by calling tryBlockTax.
        game.recordTax(p1); // Game records Alice's tax action

        // Now Bob (Governor) tries to block. This assumes Bob is not currently sanctioned and has the ability.
        // In a real game, this would be a player choice during the "reaction" phase.
        // For testing, we call the block directly if game design allows.
        CHECK(game.tryBlockTax(p2)); // Bob successfully blocks Alice's tax
        CHECK_EQ(p1->getCoins(), 4); // Alice's coins should revert to before the tax
    }

    TEST_CASE("Bribe action") {
        Game game;
        auto p1 = new Player("Alice");
        auto p2 = new Judge("Bob");
        auto p3 = new Baron("Charlie");
        game.addPlayer(p1);
        game.addPlayer(p2);
        game.addPlayer(p3);

        p1->setCoins(5); // Give Alice enough money for bribe

        // Player can bribe on their turn and pay 4 coins
        CHECK_EQ(game.turn(), "Alice");
        p1->setTurn(true);
        p1->bribe(game);
        CHECK_EQ(p1->getCoins(), 1); // 5 - 4 = 1
        CHECK(game.hasExtraTurns());
        CHECK_EQ(game.getExtraTurnsRemaining(), 2); // 2 extra turns received
        p1->setTurn(false);

        // Judge can undo the bribe, no extra turns received
        p1->setCoins(5); // Reset coins for new bribe attempt
        game.recordBribe(p1); // Record the bribe for the judge to undo
        CHECK(p2->canUndoBribe()); // Bob (Judge) can undo
        p2->undoBribe(std::shared_ptr<Player>(p1)); // Bob undoes Alice's bribe
        CHECK_EQ(p1->getCoins(), 5); // Coins should be restored if bribe undone (or not lost if concept is "prevented")
        // The current implementation of bribe deducts immediately. If undone, coins should be returned.
        // Let's assume the undoBribe mechanism handles coin refund.
        // If bribe is undone, the game should reset extra turns.
        CHECK_FALSE(game.hasExtraTurns());

        // Bribe not possible if player has less than 4 coins
        p3->setCoins(3);
        CHECK_EQ(game.turn(), "Charlie"); // Charlie's turn after Alice and Bob in this sequence.
        p3->setTurn(true);
        CHECK_THROWS_AS(p3->bribe(game), std::runtime_error);
        CHECK_EQ(p3->getCoins(), 3); // Coins should not change
        p3->setTurn(false);
    }

    TEST_CASE("Arrest action") {
        Game game;
        auto p1 = new Player("Alice"); // Arresting player
        auto p2 = new Player("Bob");   // Arrested player (normal)
        auto p3 = new Merchant("Charlie"); // Arrested player (Merchant)
        auto p4 = new Spy("David");     // Arrested player (Spy)
        game.addPlayer(p1);
        game.addPlayer(p2);
        game.addPlayer(p3);
        game.addPlayer(p4);

        p1->setCoins(5); // Give P1 some coins for actions
        p2->setCoins(5);
        p3->setCoins(5);
        p4->setCoins(5);

        // Player can arrest another player
        CHECK_EQ(game.turn(), "Alice");
        p1->setTurn(true);
        p1->arrest(std::shared_ptr<Player>(p2), game); // Alice arrests Bob
        CHECK_EQ(p2->getCoins(), 4); // Bob loses 1 coin
        CHECK_EQ(p1->getCoins(), 6); // Alice gains 1 coin
        p1->setTurn(false);
        game.clearLastArrestedFlag(); // Simulate end of turn for arrest tracking

        // Cannot arrest the same player twice in a row
        game.nextTurn(); // Bob's turn
        game.nextTurn(); // Charlie's turn
        game.nextTurn(); // David's turn
        game.nextTurn(); // Alice's turn again

        p1->setTurn(true);
        p1->arrest(std::shared_ptr<Player>(p2), game); // Alice arrests Bob again
        CHECK_EQ(p2->getCoins(), 4); // Bob should not lose another coin immediately (depends on game logic for "consecutive")
        CHECK_EQ(p1->getCoins(), 6); // Alice should not gain another coin immediately
        // The requirement is "לא ניתן להשתמש בה על אותו שחקן פעמיים ברציפות."
        // This implies that if Bob was the *last* player Alice arrested, she cannot arrest him again *on her next turn*.
        // The game should have a mechanism to track this.
        CHECK_THROWS_AS(p1->arrest(std::shared_ptr<Player>(p2), game), std::runtime_error);
        p1->setTurn(false);

        // Spy can prevent arrest
        game.clearLastArrestedFlag();
        game.nextTurn(); // David's turn

        p4->setTurn(true);
        p4->preventArrest(std::shared_ptr<Player>(p1)); // David prevents Alice from arresting
        CHECK(p1->isPreventedFromArresting());
        p4->setTurn(false);

        game.nextTurn(); // Alice's turn

        p1->setTurn(true);
        CHECK_THROWS_AS(p1->arrest(std::shared_ptr<Player>(p2), game), std::runtime_error); // Alice tries to arrest, but is prevented
        p1->setTurn(false);

        // If Merchant is arrested, they pay 2 coins to the bank instead of 1 to the attacker
        game.clearLastArrestedFlag();
        p1->setCoins(10); // Reset Alice's coins for this test
        p3->setCoins(10); // Reset Charlie's coins

        CHECK_EQ(game.turn(), "Alice");
        p1->setTurn(true);
        p1->arrest(std::shared_ptr<Player>(p3), game); // Alice arrests Charlie (Merchant)
        CHECK_EQ(p3->getCoins(), 8); // Charlie loses 2 coins
        CHECK_EQ(p1->getCoins(), 10); // Alice does NOT gain coins
        p1->setTurn(false);

        // Cannot arrest an eliminated player
        p2->eliminateMe(); // Eliminate Bob
        game.nextTurn(); // Assume it's Alice's turn
        p1->setTurn(true);
        CHECK_THROWS_AS(p1->arrest(std::shared_ptr<Player>(p2), game), std::runtime_error);
        p1->setTurn(false);
    }

    TEST_CASE("Sanction action") {
        Game game;
        auto p1 = new Player("Alice");
        auto p2 = new Baron("Bob");   // Baron for compensation check
        auto p3 = new Judge("Charlie"); // Judge for extra cost check
        game.addPlayer(p1);
        game.addPlayer(p2);
        game.addPlayer(p3);

        p1->setCoins(10); // Give Alice enough coins
        p2->setCoins(5);
        p3->setCoins(10);

        // Player can sanction another player not already sanctioned
        CHECK_EQ(game.turn(), "Alice");
        p1->setTurn(true);
        p1->sanction(std::shared_ptr<Player>(p2)); // Alice sanctions Bob
        CHECK_EQ(p1->getCoins(), 7); // Alice pays 3 coins
        CHECK(p2->isSanctioned()); // Bob is sanctioned

        // Sanction affects ability to gather or tax
        // This is tested in gather/tax test cases, but reinforce here
        p2->setTurn(true);
        p2->gather(); // Bob tries to gather while sanctioned
        CHECK_EQ(p2->getCoins(), 5); // Should not increase
        p2->tax(game); // Bob tries to tax while sanctioned
        CHECK_EQ(p2->getCoins(), 5); // Should not increase
        p2->setTurn(false);

        // If Baron is sanctioned, they get 1 coin as compensation
        // The Baron's onSanctionedBy method should handle this.
        // Let's re-run sanction with Baron as target, reset states.
        p1->setCoins(10); // Reset Alice's coins
        p2->setCoins(5); // Reset Bob's coins
        p2->setTurn(false); // Ensure Bob is not sanctioned for this new attempt
        p2->restoreFromElimination(); // Ensure Bob is alive

        p1->setTurn(true);
        p1->sanction(std::shared_ptr<Player>(p2)); // Alice sanctions Bob (Baron)
        CHECK_EQ(p1->getCoins(), 7); // Alice pays 3 coins
        CHECK_EQ(p2->getCoins(), 6); // Bob (Baron) gets 1 coin back

        // If the sanctioner is a Judge, they lose an additional coin to the bank
        p3->setCoins(10); // Reset Charlie's coins
        p1->setCoins(10); // Reset Alice's coins
        p1->setTurn(false); // Alice not sanctioned
        
        game.nextTurn(); // Ensure it's Charlie's turn.
        CHECK_EQ(game.turn(), "Charlie");
        p3->setTurn(true);
        p3->sanction(std::shared_ptr<Player>(p1)); // Charlie (Judge) sanctions Alice
        CHECK_EQ(p3->getCoins(), 6); // Charlie pays 3 (for sanction) + 1 (as Judge sanctioning) = 4 coins lost
        CHECK(p1->isSanctioned());
        p3->setTurn(false);
    }

    TEST_CASE("Coup action") {
        Game game;
        auto p1 = new Player("Alice"); // Couping player
        auto p2 = new Player("Bob");   // Coup target (normal)
        auto p3 = new General("Charlie"); // General for blocking coup
        game.addPlayer(p1);
        game.addPlayer(p2);
        game.addPlayer(p3);

        p1->setCoins(10); // Give Alice enough coins
        p2->setCoins(0);
        p3->setCoins(10); // General needs coins to block

        // Coup eliminates another player for 7 coins
        CHECK_EQ(game.turn(), "Alice");
        p1->setTurn(true);
        p1->coup(std::shared_ptr<Player>(p2), game); // Alice coups Bob
        CHECK_EQ(p1->getCoins(), 3); // Alice loses 7 coins
        CHECK_FALSE(p2->isAlive()); // Bob is eliminated
        p1->setTurn(false);
        p2->restoreFromElimination(); // Restore Bob for subsequent tests

        // Cannot coup if player has less than 7 coins
        p1->setCoins(6); // Alice has less than 7 coins
        CHECK_EQ(game.turn(), "Alice");
        p1->setTurn(true);
        CHECK_THROWS_AS(p1->coup(std::shared_ptr<Player>(p2), game), std::runtime_error);
        CHECK_EQ(p1->getCoins(), 6); // Coins should not change
        p1->setTurn(false);

        // General can block coup
        p1->setCoins(10); // Reset Alice's coins
        game.nextTurn(); // Bob's turn
        game.nextTurn(); // Charlie's turn
        
        // Alice coups Bob, Charlie (General) blocks
        p1->setTurn(true);
        p1->coup(std::shared_ptr<Player>(p2), game); // Alice attempts to coup Bob
        CHECK_EQ(p1->getCoins(), 3); // Alice initially pays 7 coins

        game.recordCoup(p1, p2); // Record the coup for the General to block
        CHECK(p3->canBlockCoup()); // Charlie (General) can block
        p3->blockCoup(std::shared_ptr<Player>(p2), std::shared_ptr<Player>(p1)); // Charlie blocks Alice's coup on Bob
        CHECK_EQ(p3->getCoins(), 5); // Charlie pays 5 coins to block
        CHECK_EQ(p1->getCoins(), 10); // Alice's coins are returned as coup is blocked
        CHECK(p2->isAlive()); // Bob is still alive
        p1->setTurn(false);

        // Cannot coup an already eliminated player
        p2->eliminateMe(); // Eliminate Bob again
        p1->setCoins(10); // Reset Alice's coins
        CHECK_EQ(game.turn(), "Alice");
        p1->setTurn(true);
        CHECK_THROWS_AS(p1->coup(std::shared_ptr<Player>(p2), game), std::runtime_error);
        CHECK_FALSE(p2->isAlive());
        p1->setTurn(false);
    }
}