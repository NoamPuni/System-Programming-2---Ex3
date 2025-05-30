#include "Player.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Game.hpp"
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

int main() {
    Game game_1;
    
    // Create players and add them to the game.
    Governor* governor = new Governor("Moshe");
    Spy* spy = new Spy("Yossi");
    Baron* baron = new Baron("Meirav");
    General* general = new General("Reut");
    Judge* judge = new Judge("Gilad");
    Merchant* merchant = new Merchant("David");
    
    // Add players to game_1.
    game_1.addPlayer(governor);
    game_1.addPlayer(spy);
    game_1.addPlayer(baron);
    game_1.addPlayer(general);
    game_1.addPlayer(judge);
    game_1.addPlayer(merchant);
    
    vector<string> players = game_1.players();
    
    cout << "=== Initial Players ===" << endl;
    // Expected output:
    // Moshe
    // Yossi
    // Meirav
    // Reut
    // Gilad
    // David
    for(string name : players){
        cout << name << endl;
    }
    
    // Expected output: Moshe
    cout << "\n=== Current Turn ===" << endl;
    cout << game_1.turn() << endl;
    
    cout << "\n=== Round 1 - Everyone Gathers ===" << endl;
    governor->gather(game_1);
    game_1.nextTurn();
    
    spy->gather(game_1);
    game_1.nextTurn();
    
    baron->gather(game_1);
    game_1.nextTurn();
    
    general->gather(game_1);
    game_1.nextTurn();
    
    judge->gather(game_1);
    game_1.nextTurn();
    
    merchant->gather(game_1);
    game_1.nextTurn();
    
    // Expected exception - Not spy's turn (it's governor's turn now).
    cout << "\n=== Testing Invalid Turn ===" << endl;
    cout << "Current turn: " << game_1.turn() << endl;
    try{
        spy->gather(game_1);
    } catch (const std::exception &e){
        std::cerr << "Exception: " << e.what() << '\n';
    }
    
    cout << "\n=== Round 2 ===" << endl;
    governor->gather(game_1);
    game_1.nextTurn();
    
    spy->tax(game_1); // Spy uses tax action.
    game_1.nextTurn();
    
    cout << "Governor coins: " << governor->getCoins() << endl; // Expected: 2.
    cout << "Spy coins: " << spy->getCoins() << endl; // Expected: 3.
    
    cout << "\n=== Testing Baron's Investment ===" << endl;
    baron->gather(game_1); // Baron gets another coin first.
    game_1.nextTurn();
    
    general->gather(game_1);
    game_1.nextTurn();
    
    judge->gather(game_1); 
    game_1.nextTurn();
    
    merchant->gather(game_1);
    game_1.nextTurn();
    
    // Now baron has 3 coins, can invest.
    cout << "Baron coins before investment: " << baron->getCoins() << endl;
    baron->invest(); // Baron trades 3 coins and gets 6.
    cout << "Baron coins after investment: " << baron->getCoins() << endl; // Expected: 6.
    game_1.nextTurn();
    
    cout << "\n=== More Rounds ===" << endl;
    spy->gather(game_1);
    game_1.nextTurn();
    
    baron->gather(game_1); // Baron gets one more coin.
    game_1.nextTurn();
    
    general->gather(game_1);
    game_1.nextTurn();
    
    judge->gather(game_1);
    game_1.nextTurn();
    
    merchant->gather(game_1);
    game_1.nextTurn();
    
    governor->tax(game_1);
    game_1.nextTurn();
    
    spy->gather(game_1);
    game_1.nextTurn();
    
    baron->gather(game_1); // Baron gets another coin to reach 7 for coup.
    game_1.nextTurn();
    
    cout << "Baron coins: " << baron->getCoins() << endl; // Should be 7 now.
    
    cout << "\n=== Testing Coup ===" << endl;
    cout << "Baron has " << baron->getCoins() << " coins before coup" << endl;
    if (baron->getCoins() >= 7) {
        baron->coup(governor, game_1); // Coup against governor.
        game_1.nextTurn();
        cout << "Coup successful!" << endl;
    } else {
        cout << "Baron doesn't have enough coins for coup, gathering more..." << endl;
        // Give baron more coins.
        baron->gather(game_1);
        game_1.nextTurn();
        general->gather(game_1);
        game_1.nextTurn();
        if (baron->getCoins() >= 7) {
            baron->coup(governor, game_1);
            game_1.nextTurn();
            cout << "Coup successful after gathering more coins!" << endl;
        }
    }
    
    cout << "\n=== Players After Coup ===" << endl;
    players = game_1.players();
    // Since no one blocked the Baron, governor should be eliminated.
    for (string name : players) {
        cout << name << endl;
    }
    
    cout << "\n=== Testing Special Abilities ===" << endl;
    
    // Test Spy's special abilities.
    spy->revealCoins(*baron);
    spy->preventArrest(*judge);
    game_1.nextTurn();
    
    // Test Merchant's special ability (gets extra coin if starting turn with 3+ coins).
    cout << "Merchant coins before turn: " << merchant->getCoins() << endl;
    merchant->onBeginTurn(); // This should give extra coin if merchant has 3+ coins.
    cout << "Merchant coins after begin turn: " << merchant->getCoins() << endl;
    game_1.nextTurn();
    
    cout << "\n=== Testing Bribe and Extra Turns ===" << endl;
    // Make sure we have enough coins for bribe test.
    while (judge->getCoins() < 4) {
        judge->gather(game_1);
        game_1.nextTurn();
        merchant->gather(game_1);
        game_1.nextTurn();
    }
    
    cout << "Judge coins before bribe: " << judge->getCoins() << endl;
    if (judge->getCoins() >= 4) {
        judge->bribe(game_1); // Judge uses bribe (costs 4 coins, gives 2 extra turns).
        cout << "Judge coins after bribe: " << judge->getCoins() << endl;
        cout << "Extra turns remaining: " << game_1.getExtraTurnsRemaining() << endl;
        
        // Judge can now do extra actions.
        judge->gather(game_1);
        judge->gather(game_1);
        game_1.nextTurn();
    }
    
    cout << "\n=== Testing Sanction ===" << endl;
    // Make sure merchant has enough coins for sanction.
    while (merchant->getCoins() < 3) {
        merchant->gather(game_1);
        game_1.nextTurn();
        if (spy->isAlive()) {
            spy->gather(game_1);
            game_1.nextTurn();
        }
    }
    
    cout << "Merchant coins before sanction: " << merchant->getCoins() << endl;
    if (merchant->getCoins() >= 3 && spy->isAlive()) {
        merchant->sanction(spy, game_1); // Merchant sanctions spy (costs 3 coins).
        cout << "Spy sanctioned successfully" << endl;
        game_1.nextTurn();
        
        // Spy is now sanctioned and cannot use economic actions.
        try {
            spy->gather(game_1); // This should fail because spy is sanctioned.
        } catch (const std::exception &e) {
            std::cerr << "Sanction working: " << e.what() << '\n';
        }
        game_1.nextTurn();
    }
    
    cout << "\n=== Game Status ===" << endl;
    cout << "Current turn: " << game_1.turn() << endl;
    cout << "Alive players: " << game_1.getAlivePlayerCount() << endl;
    cout << "Game ended: " << (game_1.isGameEnded() ? "Yes" : "No") << endl;
    
    // Clean up memory.
    delete governor;
    delete spy;
    delete baron;
    delete general;
    delete judge;
    delete merchant;
    
    return 0;
}