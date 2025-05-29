// main_gui.cpp
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <deque>
#include "Game.hpp"
#include "Player.hpp"
#include "Baron.hpp"
#include "Spy.hpp"
#include "Governor.hpp" // Include Governor.hpp for Governor specific methods
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"

// Helper function to create text elements
sf::Text createText(const std::string& content, const sf::Font& font, unsigned int size, float x, float y) {
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::White);
    text.setPosition(x, y);
    return text;
}

// Game Log variables
std::deque<std::string> gameLog;
const unsigned int MAX_LOG_MESSAGES = 15;
const float LOG_PANEL_X = 600.f; // Adjusted for new button layout
const float LOG_ENTRY_HEIGHT = 18.f;
const unsigned int LOG_FONT_SIZE = 14;

// Error Popup variables
sf::Text errorPopupTextElement;
sf::RectangleShape errorPopupBackground;
bool displayErrorPopup = false;
std::string currentErrorPopupMessage = "";
const unsigned int ERROR_FONT_SIZE = 18;

// Helper function to add messages to the game log
void addGameLogEntry(const std::string& message) {
    if (gameLog.size() >= MAX_LOG_MESSAGES) {
        gameLog.pop_front();
    }
    gameLog.push_back(message);
}

// Helper function to prepare and show an error popup
void triggerErrorPopup(const std::string& message, const sf::Font& font) {
    currentErrorPopupMessage = message;
    displayErrorPopup = true;
}

// Helper function to center text on a button
auto centerTextOnButton = [](sf::Text& text, const sf::RectangleShape& button) {
    sf::FloatRect textRect = text.getLocalBounds();
    text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    text.setPosition(button.getPosition().x + button.getSize().x / 2.0f, button.getPosition().y + button.getSize().y / 2.0f -2);
};


int main() {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return 1;
    }

    Game game;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Coup Game - GUI");
    window.setFramerateLimit(60);

    // Game states
    enum GameState { 
        ENTERING_PLAYERS, 
        PLAYING, 
        SELECTING_ARREST_TARGET, 
        SELECTING_SANCTION_TARGET, 
        SELECTING_COUP_TARGET, 
        SELECTING_SPY_TARGET, 
        BLOCKING_ACTION, // New state for blocking actions
        GAME_OVER 
    }; 
    GameState currentState = ENTERING_PLAYERS;

    std::vector<std::string> enteredPlayers;
    std::string currentPlayerName = "";
    bool gameInitialized = false;

    std::vector<sf::RectangleShape> targetButtons;
    std::vector<sf::Text> targetTexts;
    
    // Variables for blocking state
    Player* actionPerformer = nullptr; // Player who performed the action being blocked
    std::string actionTypeToBlock = ""; // e.g., "Tax", "Bribe", "Coup"
    int currentBlockerIndex = -1; // Index in _players vector of the potential blocker
    std::vector<Player*> potentialBlockers; // List of players who can block
    int pendingTaxAmount = 0; // NEW: To store the amount of coins for a pending tax action

    // Action buttons - Re-layout for 6 buttons (including Coup)
    float buttonWidth = 90.f; // Adjusted for 6 buttons
    float buttonHeight = 35.f; // Adjusted for 6 buttons
    float buttonYPos = 500.f;
    float buttonSpacing = 8.f; // Further reduced spacing
    float currentButtonX = 10.f; // Start closer to the left edge

    // Standard Actions
    sf::RectangleShape gatherButton(sf::Vector2f(buttonWidth, buttonHeight));
    gatherButton.setFillColor(sf::Color::Blue);
    gatherButton.setPosition(currentButtonX, buttonYPos);
    sf::Text gatherText = createText("Gather", font, 15, 0, 0);

    currentButtonX += buttonWidth + buttonSpacing;
    sf::RectangleShape taxButton(sf::Vector2f(buttonWidth, buttonHeight));
    taxButton.setFillColor(sf::Color::Green);
    taxButton.setPosition(currentButtonX, buttonYPos);
    sf::Text taxText = createText("Tax", font, 15, 0, 0);

    currentButtonX += buttonWidth + buttonSpacing;
    sf::RectangleShape bribeButton(sf::Vector2f(buttonWidth, buttonHeight));
    bribeButton.setFillColor(sf::Color::Magenta);
    bribeButton.setPosition(currentButtonX, buttonYPos);
    sf::Text bribeText = createText("Bribe (-4)", font, 13, 0, 0);

    currentButtonX += buttonWidth + buttonSpacing;
    sf::RectangleShape arrestButton(sf::Vector2f(buttonWidth, buttonHeight));
    arrestButton.setFillColor(sf::Color::Red);
    arrestButton.setPosition(currentButtonX, buttonYPos);
    sf::Text arrestText = createText("Arrest", font, 15, 0, 0);

    currentButtonX += buttonWidth + buttonSpacing;
    sf::RectangleShape sanctionButton(sf::Vector2f(buttonWidth, buttonHeight));
    sanctionButton.setFillColor(sf::Color(255, 165, 0)); // Orange color for Sanction
    sanctionButton.setPosition(currentButtonX, buttonYPos);
    sf::Text sanctionText = createText("Sanction(-3)", font, 12, 0, 0);

    currentButtonX += buttonWidth + buttonSpacing;
    sf::RectangleShape coupButton(sf::Vector2f(buttonWidth, buttonHeight));
    coupButton.setFillColor(sf::Color(100, 0, 100)); // Dark Purple for Coup
    coupButton.setPosition(currentButtonX, buttonYPos);
    sf::Text coupText = createText("Coup(-7)", font, 13, 0, 0);

    // Special Action Buttons
    // Baron: Invest
    sf::RectangleShape investButton(sf::Vector2f(buttonWidth, buttonHeight));
    investButton.setFillColor(sf::Color(0, 150, 150)); // Teal color
    investButton.setPosition(currentButtonX, buttonYPos - 45); // Above standard buttons
    sf::Text investText = createText("Invest (+6)", font, 13, 0, 0);

    // Spy: Spy Action (no cost, no turn)
    sf::RectangleShape spyActionButton(sf::Vector2f(buttonWidth, buttonHeight));
    spyActionButton.setFillColor(sf::Color(50, 50, 150)); // Dark Blue
    spyActionButton.setPosition(currentButtonX - buttonWidth - buttonSpacing, buttonYPos - 45); // Above standard buttons
    sf::Text spyActionText = createText("Spy Action", font, 13, 0, 0);

    // Blocking Action Buttons
    sf::RectangleShape blockButton(sf::Vector2f(120, 40));
    blockButton.setFillColor(sf::Color(200, 50, 50)); // Red for Block
    blockButton.setPosition(window.getSize().x / 2.f - 130, 450);
    sf::Text blockText = createText("Block", font, 20, 0, 0);
    centerTextOnButton(blockText, blockButton);

    sf::RectangleShape passBlockButton(sf::Vector2f(120, 40));
    passBlockButton.setFillColor(sf::Color(50, 150, 50)); // Green for Pass
    passBlockButton.setPosition(window.getSize().x / 2.f + 10, 450);
    sf::Text passBlockText = createText("Pass", font, 20, 0, 0);
    centerTextOnButton(passBlockText, passBlockButton);


    // Player entry buttons
    sf::RectangleShape enterPlayerButton(sf::Vector2f(150, 40));
    enterPlayerButton.setFillColor(sf::Color::Cyan);
    enterPlayerButton.setPosition(50, 400);
    sf::Text enterPlayerText = createText("Enter Player", font, 18, 75, 410);

    sf::RectangleShape startGameButton(sf::Vector2f(150, 40));
    startGameButton.setFillColor(sf::Color::Yellow);
    startGameButton.setPosition(250, 400);
    sf::Text startGameText = createText("Start Game", font, 18, 285, 410);
    
    // Exit Game button
    sf::RectangleShape exitGameButton(sf::Vector2f(150, 50));
    exitGameButton.setFillColor(sf::Color(150, 50, 50)); // Dark red
    exitGameButton.setPosition(window.getSize().x / 2.f - exitGameButton.getSize().x / 2.f, 400);
    sf::Text exitGameText = createText("Exit Game", font, 20, 0, 0);

    // Center texts on buttons helper (re-calling for new buttons)
    centerTextOnButton(gatherText, gatherButton);
    centerTextOnButton(taxText, taxButton);
    centerTextOnButton(bribeText, bribeButton);
    centerTextOnButton(arrestText, arrestButton);
    centerTextOnButton(sanctionText, sanctionButton);
    centerTextOnButton(coupText, coupButton);
    centerTextOnButton(investText, investButton);
    centerTextOnButton(spyActionText, spyActionButton);
    centerTextOnButton(exitGameText, exitGameButton);
    centerTextOnButton(blockText, blockButton);
    centerTextOnButton(passBlockText, passBlockButton);

    bool actionPerformedThisClick = false; // Prevents multiple actions on single click

    errorPopupBackground.setFillColor(sf::Color(70, 70, 70, 230));
    errorPopupBackground.setOutlineColor(sf::Color::Red);
    errorPopupBackground.setOutlineThickness(2.f);

    addGameLogEntry("Welcome to Coup! Enter player names.");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (displayErrorPopup) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    displayErrorPopup = false;
                    actionPerformedThisClick = true; 
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    displayErrorPopup = false;
                    actionPerformedThisClick = true; 
                }
                if (actionPerformedThisClick && (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::KeyPressed)) {
                    if(event.type == sf::Event::MouseButtonReleased) actionPerformedThisClick = false;
                    continue; 
                }
            }

            if (game.isGameEnded()) {
                if (currentState != GAME_OVER) {
                    currentState = GAME_OVER;
                    addGameLogEntry("Game Over! Winner: " + game.winner());
                }
                if (event.type == sf::Event::MouseButtonPressed && !actionPerformedThisClick) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    if (exitGameButton.getGlobalBounds().contains(mousePos)) {
                        window.close();
                    }
                    actionPerformedThisClick = true;
                }
                if (event.type == sf::Event::MouseButtonReleased) {
                    actionPerformedThisClick = false;
                }
                continue;
            }

            if (currentState == ENTERING_PLAYERS) {
                 if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode < 128) {
                        if (event.text.unicode == '\b') { 
                            if (!currentPlayerName.empty()) { currentPlayerName.pop_back(); }
                        } else if (event.text.unicode == '\r' || event.text.unicode == '\n') {
                            if (!currentPlayerName.empty() && enteredPlayers.size() < 6) {
                                enteredPlayers.push_back(currentPlayerName);
                                addGameLogEntry("Player added: " + currentPlayerName);
                                currentPlayerName = "";
                            }
                        } else if (event.text.unicode != ' ' || !currentPlayerName.empty()) {
                            currentPlayerName += static_cast<char>(event.text.unicode);
                        }
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed && !actionPerformedThisClick) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    if (enterPlayerButton.getGlobalBounds().contains(mousePos)) {
                        if (!currentPlayerName.empty() && enteredPlayers.size() < 6) {
                            enteredPlayers.push_back(currentPlayerName);
                            addGameLogEntry("Player added: " + currentPlayerName);
                            currentPlayerName = "";
                        } else if (currentPlayerName.empty()) { addGameLogEntry("Cannot add empty name.");}
                        else { addGameLogEntry("Max players (6) reached.");}
                        actionPerformedThisClick = true;
                    } else if (startGameButton.getGlobalBounds().contains(mousePos)) {
                        if (enteredPlayers.size() >= 2) {
                            try {
                                game.initializeGame(enteredPlayers);
                                gameInitialized = true;
                                currentState = PLAYING;
                                addGameLogEntry("Game started with " + std::to_string(enteredPlayers.size()) + " players!");
                            } catch (const std::exception& e) {
                                triggerErrorPopup("Init game error: " + std::string(e.what()), font);
                            }
                        } else { addGameLogEntry("Need 2-6 players."); }
                        actionPerformedThisClick = true;
                    }
                }
            }
            else if (currentState == PLAYING) {
                if (event.type == sf::Event::MouseButtonPressed && !actionPerformedThisClick) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    Player* currentPlayer = game.getCurrentPlayer();

                    if (currentPlayer != nullptr && currentPlayer->isAlive()) {
                        try {
                            // Standard Actions
                            if (gatherButton.getGlobalBounds().contains(mousePos)) {
                                currentPlayer->gather(game); // Pass game reference
                                addGameLogEntry(currentPlayer->getName() + " gathered 1 coin.");
                                game.nextTurn();
                                actionPerformedThisClick = true;
                            }
                            else if (taxButton.getGlobalBounds().contains(mousePos)) {
                                // NEW: Get the amount first, but don't add coins yet.
                                pendingTaxAmount = currentPlayer->tax(game); // Pass game reference
                                game.recordTax(currentPlayer, pendingTaxAmount); // Record the tax action with amount
                                
                                addGameLogEntry(currentPlayer->getName() + " performed tax (pending " + std::to_string(pendingTaxAmount) + " coins).");
                                
                                // Check for potential blockers
                                potentialBlockers.clear();
                                for (Player* p : game.getAllPlayers()) {
                                    if (p->isAlive() && p != currentPlayer && p->canBlockTax()) {
                                        potentialBlockers.push_back(p);
                                    }
                                }

                                if (!potentialBlockers.empty()) {
                                    currentBlockerIndex = 0; // Start with the first potential blocker
                                    actionPerformer = currentPlayer;
                                    actionTypeToBlock = "Tax";
                                    currentState = BLOCKING_ACTION;
                                    addGameLogEntry("A Tax action was performed. Checking for blocks...");
                                } else {
                                    // NEW: No one can block, so apply the tax immediately
                                    currentPlayer->setCoins(pendingTaxAmount);
                                    addGameLogEntry(currentPlayer->getName() + " received " + std::to_string(pendingTaxAmount) + " coins from Tax.");
                                    game.nextTurn(); // Proceed to next turn
                                    pendingTaxAmount = 0; // Reset
                                }
                                actionPerformedThisClick = true;
                            }
                            else if (bribeButton.getGlobalBounds().contains(mousePos)) {
                                // game.recordBribe(currentPlayer); // If bribe becomes blockable
                                currentPlayer->bribe(game);
                                addGameLogEntry(currentPlayer->getName() + " performed bribe.");
                                game.nextTurn(); 
                                actionPerformedThisClick = true;
                            }
                            else if (arrestButton.getGlobalBounds().contains(mousePos)) {
                                if (!currentPlayer->isPreventedFromArresting()) { // Check if prevented
                                    addGameLogEntry(currentPlayer->getName() + " chooses Arrest target...");
                                    currentState = SELECTING_ARREST_TARGET;
                                    targetButtons.clear(); targetTexts.clear();
                                    auto allPlayers = game.getAllPlayers(); // Assuming getAllPlayers exists and returns Player*
                                    float yPos = 150;
                                    for (Player* p : allPlayers) {
                                        if (p->isAlive() && p != currentPlayer) {
                                            sf::RectangleShape targetBtn(sf::Vector2f(200, 40));
                                            targetBtn.setFillColor(sf::Color::Yellow);
                                            targetBtn.setPosition(window.getSize().x / 2.f - targetBtn.getSize().x / 2.f, yPos);
                                            targetButtons.push_back(targetBtn);
                                            sf::Text targetTxt = createText(p->getName(), font, 18, 0, 0);
                                            centerTextOnButton(targetTxt, targetBtn);
                                            targetTexts.push_back(targetTxt);
                                            yPos += 50;
                                        }
                                    }
                                    if (targetButtons.empty()) {
                                        addGameLogEntry("No valid targets for Arrest.");
                                        currentState = PLAYING;
                                    }
                                } else {
                                    triggerErrorPopup("You are prevented from using Arrest this turn!", font);
                                }
                                actionPerformedThisClick = true;
                            }
                            else if (sanctionButton.getGlobalBounds().contains(mousePos)) {
                                if (currentPlayer->getCoins() >= 3) {
                                    addGameLogEntry(currentPlayer->getName() + " chooses Sanction target...");
                                    currentState = SELECTING_SANCTION_TARGET;
                                    targetButtons.clear(); targetTexts.clear();
                                    auto allPlayers = game.getAllPlayers();
                                    float yPos = 150;
                                    for (Player* p : allPlayers) {
                                        if (p->isAlive() && p != currentPlayer) {
                                            sf::RectangleShape targetBtn(sf::Vector2f(200, 40));
                                            targetBtn.setFillColor(sf::Color(255, 165, 0)); // Orange color for Sanction
                                            targetBtn.setPosition(window.getSize().x / 2.f - targetBtn.getSize().x / 2.f, yPos);
                                            targetButtons.push_back(targetBtn);
                                            sf::Text targetTxt = createText(p->getName(), font, 18, 0, 0);
                                            centerTextOnButton(targetTxt, targetBtn);
                                            targetTexts.push_back(targetTxt);
                                            yPos += 50;
                                        }
                                    }
                                    if (targetButtons.empty()) {
                                        addGameLogEntry("No valid targets for Sanction.");
                                        currentState = PLAYING;
                                    }
                                } else {
                                    triggerErrorPopup("Not enough coins for Sanction (need 3).", font);
                                }
                                actionPerformedThisClick = true;
                            }
                            else if (coupButton.getGlobalBounds().contains(mousePos)) {
                                if (currentPlayer->getCoins() >= 7) {
                                    // game.recordCoup(currentPlayer, targetPlayer); // If coup becomes blockable
                                    addGameLogEntry(currentPlayer->getName() + " chooses Coup target...");
                                    currentState = SELECTING_COUP_TARGET;
                                    targetButtons.clear(); targetTexts.clear();
                                    auto allPlayers = game.getAllPlayers();
                                    float yPos = 150;
                                    for (Player* p : allPlayers) {
                                        if (p->isAlive() && p != currentPlayer) {
                                            sf::RectangleShape targetBtn(sf::Vector2f(200, 40));
                                            targetBtn.setFillColor(sf::Color(100, 0, 100)); // Dark Purple
                                            targetBtn.setPosition(window.getSize().x / 2.f - targetBtn.getSize().x / 2.f, yPos);
                                            targetButtons.push_back(targetBtn);
                                            sf::Text targetTxt = createText(p->getName(), font, 18, 0, 0);
                                            centerTextOnButton(targetTxt, targetBtn);
                                            targetTexts.push_back(targetTxt);
                                            yPos += 50;
                                        }
                                    }
                                    if (targetButtons.empty()) {
                                        addGameLogEntry("No valid targets for Coup.");
                                        currentState = PLAYING;
                                    }
                                } else {
                                    triggerErrorPopup("Not enough coins for Coup (need 7).", font);
                                }
                                actionPerformedThisClick = true;
                            }
                            // Special Actions (Baron, Spy)
                            else if (currentPlayer->role() == "Baron" && investButton.getGlobalBounds().contains(mousePos)) {
                                Baron* baronPlayer = dynamic_cast<Baron*>(currentPlayer);
                                if (baronPlayer) {
                                    baronPlayer->invest(); 
                                    addGameLogEntry(currentPlayer->getName() + " (Baron) invested and gained 6 coins.");
                                } else {
                                    triggerErrorPopup("Internal Error: Player is not a Baron despite role() returning Baron.", font);
                                }
                                actionPerformedThisClick = true;
                            }
                            else if (currentPlayer->role() == "Spy" && spyActionButton.getGlobalBounds().contains(mousePos)) {
                                addGameLogEntry(currentPlayer->getName() + " (Spy) chooses Spy Action target...");
                                currentState = SELECTING_SPY_TARGET;
                                targetButtons.clear(); targetTexts.clear();
                                auto allPlayers = game.getAllPlayers();
                                float yPos = 150;
                                for (Player* p : allPlayers) {
                                    if (p->isAlive() && p != currentPlayer) {
                                        sf::RectangleShape targetBtn(sf::Vector2f(200, 40));
                                        targetBtn.setFillColor(sf::Color(50, 50, 150));
                                        targetBtn.setPosition(window.getSize().x / 2.f - targetBtn.getSize().x / 2.f, yPos);
                                        targetButtons.push_back(targetBtn);
                                        sf::Text targetTxt = createText(p->getName(), font, 18, 0, 0);
                                        centerTextOnButton(targetTxt, targetBtn);
                                        targetTexts.push_back(targetTxt);
                                        yPos += 50;
                                    }
                                }
                                if (targetButtons.empty()) {
                                    addGameLogEntry("No valid targets for Spy Action.");
                                    currentState = PLAYING;
                                }
                                actionPerformedThisClick = true;
                            }

                        } catch (const std::exception& e) {
                            triggerErrorPopup("Action Error: " + std::string(e.what()), font);
                            actionPerformedThisClick = true;
                        }
                    } else if (currentPlayer && !currentPlayer->isAlive()) {
                         addGameLogEntry("Eliminated players cannot act.");
                         actionPerformedThisClick = true;
                    }
                }
            }
            else if (currentState == SELECTING_ARREST_TARGET) {
                if (event.type == sf::Event::MouseButtonPressed && !actionPerformedThisClick) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    Player* currentPlayer = game.getCurrentPlayer();
                    if (currentPlayer != nullptr) {
                        for (size_t i = 0; i < targetButtons.size(); ++i) {
                            if (targetButtons[i].getGlobalBounds().contains(mousePos)) {
                                std::string targetName = targetTexts[i].getString();
                                try {
                                    Player* targetPlayer = nullptr;
                                    for (Player* p : game.getAllPlayers()) { if (p->getName() == targetName) { targetPlayer = p; break; } }
                                    
                                    if (targetPlayer != nullptr && targetPlayer->isAlive()) {
                                        bool arrestSuccessful = currentPlayer->arrest(targetPlayer, game);
                                        if (arrestSuccessful) {
                                            addGameLogEntry(currentPlayer->getName() + " arrested " + targetName + "!");
                                            game.nextTurn(); 
                                        } else {
                                            triggerErrorPopup("Arrest on " + targetName + " failed (e.g., recently arrested or prevented).", font);
                                        }
                                    } else { triggerErrorPopup("Target " + targetName + " is invalid or not found.", font); }
                                } catch (const std::exception& e) { triggerErrorPopup("Arrest Error: " + std::string(e.what()), font); }
                                currentState = PLAYING;
                                actionPerformedThisClick = true;
                                break;
                            }
                        }
                    }
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    currentState = PLAYING; addGameLogEntry("Arrest action cancelled."); actionPerformedThisClick = true;
                }
            }
            else if (currentState == SELECTING_SANCTION_TARGET) {
                if (event.type == sf::Event::MouseButtonPressed && !actionPerformedThisClick) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    Player* currentPlayer = game.getCurrentPlayer();
                    if (currentPlayer != nullptr) {
                        for (size_t i = 0; i < targetButtons.size(); ++i) {
                            if (targetButtons[i].getGlobalBounds().contains(mousePos)) {
                                std::string targetName = targetTexts[i].getString();
                                try {
                                    Player* targetPlayer = nullptr;
                                    for (Player* p : game.getAllPlayers()) { if (p->getName() == targetName) { targetPlayer = p; break; } }

                                    if (targetPlayer != nullptr && targetPlayer->isAlive()) {
                                        currentPlayer->sanction(targetPlayer, game); // Pass game reference
                                        addGameLogEntry(currentPlayer->getName() + " sanctioned " + targetName + ".");
                                        game.nextTurn();
                                    } else { triggerErrorPopup("Target " + targetName + " is invalid or not found.", font); }
                                } catch (const std::exception& e) {
                                    triggerErrorPopup("Sanction Error: " + std::string(e.what()), font);
                                }
                                currentState = PLAYING;
                                actionPerformedThisClick = true;
                                break;
                            }
                        }
                    } else if (currentPlayer && currentPlayer->getCoins() < 3) {
                        triggerErrorPopup("Not enough coins for Sanction (need 3).", font);
                        currentState = PLAYING;
                        actionPerformedThisClick = true;
                    }
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    currentState = PLAYING; addGameLogEntry("Sanction action cancelled."); actionPerformedThisClick = true;
                }
            }
            else if (currentState == SELECTING_COUP_TARGET) {
                if (event.type == sf::Event::MouseButtonPressed && !actionPerformedThisClick) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    Player* currentPlayer = game.getCurrentPlayer();
                    if (currentPlayer != nullptr) {
                        for (size_t i = 0; i < targetButtons.size(); ++i) {
                            if (targetButtons[i].getGlobalBounds().contains(mousePos)) {
                                std::string targetName = targetTexts[i].getString();
                                try {
                                    Player* targetPlayer = nullptr;
                                    for (Player* p : game.getAllPlayers()) { if (p->getName() == targetName) { targetPlayer = p; break; } }

                                    if (targetPlayer != nullptr && targetPlayer->isAlive()) {
                                        currentPlayer->coup(targetPlayer, game); 
                                        addGameLogEntry(currentPlayer->getName() + " performed Coup on " + targetName + "!");
                                        game.nextTurn();
                                    } else { triggerErrorPopup("Target " + targetName + " is invalid or not found.", font); }
                                } catch (const std::exception& e) {
                                    triggerErrorPopup("Coup Error: " + std::string(e.what()), font);
                                }
                                currentState = PLAYING;
                                actionPerformedThisClick = true;
                                break;
                            }
                        }
                    } else if (currentPlayer && currentPlayer->getCoins() < 7) {
                        triggerErrorPopup("Not enough coins for Coup (need 7).", font);
                        currentState = PLAYING;
                        actionPerformedThisClick = true;
                    }
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    currentState = PLAYING; addGameLogEntry("Coup action cancelled."); actionPerformedThisClick = true;
                }
            }
            else if (currentState == SELECTING_SPY_TARGET) {
                if (event.type == sf::Event::MouseButtonPressed && !actionPerformedThisClick) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    Player* currentPlayer = game.getCurrentPlayer();
                    if (currentPlayer != nullptr && currentPlayer->role() == "Spy") {
                        for (size_t i = 0; i < targetButtons.size(); ++i) {
                            if (targetButtons[i].getGlobalBounds().contains(mousePos)) {
                                std::string targetName = targetTexts[i].getString();
                                try {
                                    Player* targetPlayer = nullptr;
                                    for (Player* p : game.getAllPlayers()) { if (p->getName() == targetName) { targetPlayer = p; break; } }

                                    if (targetPlayer != nullptr && targetPlayer->isAlive()) {
                                        Spy* spyPlayer = dynamic_cast<Spy*>(currentPlayer);
                                        if (spyPlayer) {
                                            spyPlayer->revealCoins(*targetPlayer);
                                            spyPlayer->preventArrest(*targetPlayer);
                                            addGameLogEntry(currentPlayer->getName() + " (Spy) used Spy Action on " + targetName + "!");
                                            addGameLogEntry(targetName + " has " + std::to_string(targetPlayer->getCoins()) + " coins.");
                                            addGameLogEntry(targetName + " cannot use Arrest next turn.");
                                        } else {
                                            triggerErrorPopup("Internal Error: Player is not a Spy despite role() returning Spy.", font);
                                        }
                                    } else { triggerErrorPopup("Target " + targetName + " is invalid or not found.", font); }
                                } catch (const std::exception& e) {
                                    triggerErrorPopup("Spy Action Error: " + std::string(e.what()), font);
                                }
                                currentState = PLAYING; // Spy action doesn't end turn
                                actionPerformedThisClick = true;
                                break;
                            }
                        }
                    }
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    currentState = PLAYING; addGameLogEntry("Spy Action cancelled."); actionPerformedThisClick = true;
                }
            }
            else if (currentState == BLOCKING_ACTION) {
                if (event.type == sf::Event::MouseButtonPressed && !actionPerformedThisClick) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    
                    if (currentBlockerIndex < potentialBlockers.size()) {
                        Player* currentBlocker = potentialBlockers[currentBlockerIndex];

                        if (blockButton.getGlobalBounds().contains(mousePos)) {
                            bool blocked = false;
                            try {
                                if (actionTypeToBlock == "Tax") {
                                    // game.tryBlockTax returns true if blocked. It handles undoing the tax logic.
                                    blocked = game.tryBlockTax(currentBlocker); 
                                }
                                // Add other blockable actions here (e.g., Bribe, Coup)
                            } catch (const std::exception& e) {
                                triggerErrorPopup("Block Error: " + std::string(e.what()), font);
                            }

                            if (blocked) {
                                // If blocked, the action was effectively undone by game.tryBlockTax
                                // The original tax amount is set to 0 within game.tryBlockTax
                                addGameLogEntry("Tax action BLOCKED by " + currentBlocker->getName() + "!");
                                pendingTaxAmount = 0; // Ensure no coins are added after a successful block
                                game.nextTurn(); // Proceed to next turn
                                currentState = PLAYING;
                            } else {
                                // If block attempt failed (e.g., not correct role), move to next blocker or action stands
                                addGameLogEntry(currentBlocker->getName() + " tried to block, but failed or cannot block.");
                                currentBlockerIndex++;
                                if (currentBlockerIndex >= potentialBlockers.size()) {
                                    addGameLogEntry("No more players can block this action. Action stands.");
                                    // NEW: Apply the tax since no one blocked it successfully
                                    actionPerformer->setCoins(pendingTaxAmount);
                                    addGameLogEntry(actionPerformer->getName() + " received " + std::to_string(pendingTaxAmount) + " coins from Tax.");
                                    game.nextTurn();
                                    currentState = PLAYING;
                                    pendingTaxAmount = 0; // Reset
                                } else {
                                    addGameLogEntry("Moving to next potential blocker: " + potentialBlockers[currentBlockerIndex]->getName());
                                }
                            }
                            actionPerformedThisClick = true;
                        }
                        else if (passBlockButton.getGlobalBounds().contains(mousePos)) {
                            addGameLogEntry(currentBlocker->getName() + " chose NOT to block " + actionTypeToBlock + ".");
                            currentBlockerIndex++; // Move to next potential blocker
                            if (currentBlockerIndex >= potentialBlockers.size()) {
                                addGameLogEntry("No more players can block this action. Action stands.");
                                // NEW: Apply the tax since all players passed
                                actionPerformer->setCoins(pendingTaxAmount);
                                addGameLogEntry(actionPerformer->getName() + " received " + std::to_string(pendingTaxAmount) + " coins from Tax.");
                                game.nextTurn(); // Action stands, proceed to next turn
                                currentState = PLAYING;
                                pendingTaxAmount = 0; // Reset
                            } else {
                                addGameLogEntry("Moving to next potential blocker: " + potentialBlockers[currentBlockerIndex]->getName());
                            }
                            actionPerformedThisClick = true;
                        }
                    }
                }
            }


            if (event.type == sf::Event::MouseButtonReleased) {
                actionPerformedThisClick = false;
            }
        }

        window.clear(sf::Color(30, 30, 30));

        // === Draw game elements based on state ===
        // Player Entry Screen
        if (currentState == ENTERING_PLAYERS) {
            sf::Text titleText = createText("Enter Player Names (2-6)", font, 28, 50, 50);
            window.draw(titleText);
            std::string inputDisplay = "Current: " + currentPlayerName + (currentPlayerName.length() > 0 ? "_" : "");
            sf::Text inputText = createText(inputDisplay, font, 20, 50, 120);
            window.draw(inputText);
            sf::Text playersLabel = createText("Players:", font, 20, 50, 160);
            window.draw(playersLabel);
            for (size_t i = 0; i < enteredPlayers.size(); ++i) {
                std::string playerDisplay = std::to_string(i + 1) + ". " + enteredPlayers[i];
                sf::Text playerTextToDraw = createText(playerDisplay, font, 18, 70, 190 + i * 25);
                window.draw(playerTextToDraw);
            }
            std::string instruction = "Players: " + std::to_string(enteredPlayers.size()) + "/6";
            sf::Text instructionText = createText(instruction, font, 16, 50, 340);
            window.draw(instructionText);
            sf::Text instructionText2 = createText("Press Enter or click 'Enter Player'", font, 14, 50, 360);
            window.draw(instructionText2);
            window.draw(enterPlayerButton);
            window.draw(enterPlayerText);
            if (enteredPlayers.size() >= 2) {
                window.draw(startGameButton);
                window.draw(startGameText);
            }
        }
        // Playing, Selecting Target, Blocking Action Screens
        else if (currentState == PLAYING || currentState == SELECTING_ARREST_TARGET || currentState == SELECTING_SANCTION_TARGET || currentState == SELECTING_COUP_TARGET || currentState == SELECTING_SPY_TARGET || currentState == BLOCKING_ACTION) {
            std::string turnTitleStr = "Turn: " + game.turn();
            if (currentState == SELECTING_ARREST_TARGET) turnTitleStr += " - Select Arrest Target (ESC to cancel)";
            if (currentState == SELECTING_SANCTION_TARGET) turnTitleStr += " - Select Sanction Target (ESC to cancel)";
            if (currentState == SELECTING_COUP_TARGET) turnTitleStr += " - Select Coup Target (ESC to cancel)";
            if (currentState == SELECTING_SPY_TARGET) turnTitleStr += " - Select Spy Action Target (ESC to cancel)";
            if (currentState == BLOCKING_ACTION) {
                 if (currentBlockerIndex != -1 && currentBlockerIndex < potentialBlockers.size()) {
                    turnTitleStr = "BLOCK OPPORTUNITY: " + potentialBlockers[currentBlockerIndex]->getName();
                    turnTitleStr += " (" + potentialBlockers[currentBlockerIndex]->role() + ") can block " + actionPerformer->getName() + "'s " + actionTypeToBlock + "!";
                 } else {
                     turnTitleStr = "Resolving Action..."; // Fallback if index is out of bounds
                 }
            }
            sf::Text turnText = createText(turnTitleStr, font, 22, 20, 20);
            window.draw(turnText);

            float yPlayerInfo = 60;
            Player* gameCurrentPlayer = game.getCurrentPlayer();
            for (Player* p : game.getAllPlayers()) { // Assuming getAllPlayers exists
                std::ostringstream oss;
                oss << p->getName();
                if (p->isAlive()) {
                    oss << " (" << p->role() << ")";
                    oss << " - Coins: " << p->getCoins();
                    if (p->isSanctioned()) {
                        oss << " [S]";
                    }
                    if (p->isPreventedFromArresting()) { // Indicate if prevented from arresting
                        oss << " [No Arrest]";
                    }
                } else {
                    oss << " (" << p->role() << ") - Coins: " << p->getCoins() << " (Eliminated)";
                }

                sf::Text playerTextToDraw = createText(oss.str(), font, 16, 20, yPlayerInfo);
                // Highlight current player only if it's their turn for main actions
                if (currentState == PLAYING && p == gameCurrentPlayer && p->isAlive()) { 
                    playerTextToDraw.setFillColor(sf::Color::Yellow); 
                }
                // Highlight potential blocker in BLOCKING_ACTION state
                else if (currentState == BLOCKING_ACTION && currentBlockerIndex != -1 && p == potentialBlockers[currentBlockerIndex]) {
                     playerTextToDraw.setFillColor(sf::Color::Cyan); // Highlight active blocker
                }
                else if (!p->isAlive()) { playerTextToDraw.setFillColor(sf::Color(128,128,128)); }
                window.draw(playerTextToDraw);
                yPlayerInfo += 25;
            }

            if (currentState == PLAYING) {
                window.draw(gatherButton); window.draw(gatherText);
                window.draw(taxButton); window.draw(taxText);
                window.draw(bribeButton); window.draw(bribeText);
                window.draw(arrestButton); window.draw(arrestText);
                window.draw(sanctionButton); window.draw(sanctionText);
                window.draw(coupButton); window.draw(coupText);
                
                // Draw special action buttons only for the relevant player
                if (gameCurrentPlayer != nullptr) {
                    if (gameCurrentPlayer->role() == "Baron") {
                        window.draw(investButton); window.draw(investText);
                    }
                    if (gameCurrentPlayer->role() == "Spy") {
                        window.draw(spyActionButton); window.draw(spyActionText);
                    }
                }

                sf::Text instructionText = createText("Choose an action:", font, 18, 50, buttonYPos - 40.f);
                window.draw(instructionText);
            } else if (currentState == BLOCKING_ACTION) {
                sf::Text instructionText = createText("Do you want to block this action?", font, 18, window.getSize().x / 2.f - 150, 380);
                window.draw(instructionText);
                window.draw(blockButton);
                window.draw(blockText);
                window.draw(passBlockButton);
                window.draw(passBlockText);
            }
            else { // SELECTING_TARGET states
                std::string selectInstruction = "";
                if (currentState == SELECTING_ARREST_TARGET) {
                    selectInstruction = "Click player to Arrest (takes 1 coin)";
                } else if (currentState == SELECTING_SANCTION_TARGET) {
                    selectInstruction = "Click player to Sanction (costs 3 coins)";
                }
                else if (currentState == SELECTING_COUP_TARGET) {
                    selectInstruction = "Click player to Coup (costs 7 coins, eliminates them)";
                }
                else if (currentState == SELECTING_SPY_TARGET) { // New instruction for Spy
                    selectInstruction = "Click player to Spy on (no cost, no turn)";
                }
                sf::Text instructionText = createText(selectInstruction, font, 16, 50, 120);
                window.draw(instructionText);
                for (size_t i = 0; i < targetButtons.size(); ++i) {
                    window.draw(targetButtons[i]);
                    window.draw(targetTexts[i]);
                }
            }
        }
        // Game Over Screen
        else if (currentState == GAME_OVER) {
            sf::Text gameOverText = createText("Game Over!", font, 40, window.getSize().x / 2.f, 150);
            gameOverText.setOrigin(gameOverText.getLocalBounds().width / 2.f, gameOverText.getLocalBounds().height / 2.f);
            window.draw(gameOverText);

            std::string winnerMessage = "Winner: " + game.winner() + "!";
            sf::Text winnerText = createText(winnerMessage, font, 30, window.getSize().x / 2.f, 250);
            winnerText.setOrigin(winnerText.getLocalBounds().width / 2.f, winnerText.getLocalBounds().height / 2.0f);
            window.draw(winnerText);

            window.draw(exitGameButton);
            window.draw(exitGameText);
        }

        // Draw Game Log
        if (currentState != GAME_OVER) { // Don't draw log on game over screen if it overlaps too much
            float currentLogY = 20.f;
            sf::Text logTextElementProto;
            logTextElementProto.setFont(font);
            logTextElementProto.setCharacterSize(LOG_FONT_SIZE);
            logTextElementProto.setFillColor(sf::Color(200, 200, 200)); 
            sf::RectangleShape logAreaBg(sf::Vector2f(window.getSize().x - LOG_PANEL_X, window.getSize().y));
            logAreaBg.setPosition(LOG_PANEL_X, 0);
            logAreaBg.setFillColor(sf::Color(40, 40, 40, 180));
            window.draw(logAreaBg);
            for (const auto& message : gameLog) {
                logTextElementProto.setString(message);
                logTextElementProto.setPosition(LOG_PANEL_X + 5.f, currentLogY);
                window.draw(logTextElementProto);
                currentLogY += LOG_ENTRY_HEIGHT;
                if (currentLogY > window.getSize().y - LOG_ENTRY_HEIGHT) break;
            }
        }

        // Draw Error Popup
        if (displayErrorPopup) {
            errorPopupTextElement.setFont(font);
            errorPopupTextElement.setString(currentErrorPopupMessage);
            errorPopupTextElement.setCharacterSize(ERROR_FONT_SIZE);
            errorPopupTextElement.setFillColor(sf::Color::White);
            sf::FloatRect textBounds = errorPopupTextElement.getLocalBounds();
            float maxWidth = window.getSize().x * 0.8f;
            float textWidth = textBounds.width;
            if (textWidth > maxWidth) textWidth = maxWidth; 
            float padding = 25.f;
            errorPopupBackground.setSize(sf::Vector2f(textWidth + 2 * padding, textBounds.height + 2 * padding + 15));
            errorPopupBackground.setOrigin(errorPopupBackground.getSize().x / 2.f, errorPopupBackground.getSize().y / 2.f);
            errorPopupBackground.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
            errorPopupTextElement.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
            errorPopupTextElement.setPosition(errorPopupBackground.getPosition());
            window.draw(errorPopupBackground);
            window.draw(errorPopupTextElement);
        }

        window.display();
    }
    return 0;
}