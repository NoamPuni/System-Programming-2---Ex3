// main_gui.cpp

//************************************************************************************** */

// As permitted by the course lecturers, I used AI tools to assist in writing this GUI code.

//************************************************************************************** */

// This is a simple GUI for the Coup game using SFML.
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <deque>
#include <map> // Added for playerBoxes
#include "Game.hpp"
#include "Player.hpp"
#include "Baron.hpp"
#include "Spy.hpp"
#include "Governor.hpp"
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
const float LOG_PANEL_X = 600.f;
const float LOG_ENTRY_HEIGHT = 18.f;
const unsigned int LOG_FONT_SIZE = 14;

// Error Popup variables
sf::Text errorPopupTextElement;
sf::RectangleShape errorPopupBackground;
bool displayErrorPopup = false;
std::string currentErrorPopupMessage = "";
const unsigned int ERROR_FONT_SIZE = 18;

// Blocking mechanism variables
bool blockPending = false;
Player* actionPerformer = nullptr;
Player* actionTarget = nullptr;
std::string actionTypeToBlock = "";
Player* potentialBlocker = nullptr;
int blockCost = 0;

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
    text.setPosition(button.getPosition().x + button.getSize().x / 2.0f, button.getPosition().y + button.getSize().y / 2.0f - 2);
};

int main() {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font 'arial.ttf'! Ensure the file is in the correct directory." << std::endl;
        return 1;
    }

    Game game;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Coup Game - GUI");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(false); // Disable vertical sync to avoid warning

    // Game states
    enum GameState {
        ENTERING_PLAYERS,
        PLAYING,
        SELECTING_ARREST_TARGET,
        SELECTING_SANCTION_TARGET,
        SELECTING_COUP_TARGET,
        SELECTING_SPY_TARGET,
        BLOCKING_ACTION,
        GAME_OVER
    };
    GameState currentState = ENTERING_PLAYERS;

    std::vector<std::string> enteredPlayers;
    std::string currentPlayerName = "";
    bool gameInitialized = false;

    std::map<std::string, sf::RectangleShape> playerBoxes; // Map to store player rectangles

    // Action buttons
    float buttonWidth = 90.f;
    float buttonHeight = 35.f;
    float buttonYPos = 500.f;
    float buttonSpacing = 8.f;
    float currentButtonX = 10.f;

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
    sanctionButton.setFillColor(sf::Color(255, 165, 0));
    sanctionButton.setPosition(currentButtonX, buttonYPos);
    sf::Text sanctionText = createText("Sanction(-3)", font, 12, 0, 0);

    currentButtonX += buttonWidth + buttonSpacing;
    sf::RectangleShape coupButton(sf::Vector2f(buttonWidth, buttonHeight));
    coupButton.setFillColor(sf::Color(100, 0, 100));
    coupButton.setPosition(currentButtonX, buttonYPos);
    sf::Text coupText = createText("Coup(-7)", font, 13, 0, 0);

    // Special Action Buttons
    sf::RectangleShape investButton(sf::Vector2f(buttonWidth, buttonHeight));
    investButton.setFillColor(sf::Color(0, 150, 150));
    investButton.setPosition(currentButtonX, buttonYPos - 45);
    sf::Text investText = createText("Invest (+6)", font, 13, 0, 0);

    sf::RectangleShape spyActionButton(sf::Vector2f(buttonWidth, buttonHeight));
    spyActionButton.setFillColor(sf::Color(50, 50, 150));
    spyActionButton.setPosition(currentButtonX - buttonWidth - buttonSpacing, buttonYPos - 45);
    sf::Text spyActionText = createText("Spy Action", font, 13, 0, 0);

    // Blocking Action Buttons
    sf::RectangleShape blockButton(sf::Vector2f(120, 40));
    blockButton.setFillColor(sf::Color(200, 50, 50));
    blockButton.setPosition(window.getSize().x / 2.f - 130, 450);
    sf::Text blockButtonText = createText("Block", font, 20, 0, 0);
    centerTextOnButton(blockButtonText, blockButton);

    sf::RectangleShape skipBlockButton(sf::Vector2f(120, 40));
    skipBlockButton.setFillColor(sf::Color(50, 150, 50));
    skipBlockButton.setPosition(window.getSize().x / 2.f + 10, 450);
    sf::Text skipBlockButtonText = createText("Don't Block", font, 20, 0, 0);
    centerTextOnButton(skipBlockButtonText, skipBlockButton);

    sf::Text blockPromptText;

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
    exitGameButton.setFillColor(sf::Color(150, 50, 50));
    exitGameButton.setPosition(window.getSize().x / 2.f - exitGameButton.getSize().x / 2.f, 400);
    sf::Text exitGameText = createText("Exit Game", font, 20, 0, 0);

    // Center texts on buttons
    centerTextOnButton(gatherText, gatherButton);
    centerTextOnButton(taxText, taxButton);
    centerTextOnButton(bribeText, bribeButton);
    centerTextOnButton(arrestText, arrestButton);
    centerTextOnButton(sanctionText, sanctionButton);
    centerTextOnButton(coupText, coupButton);
    centerTextOnButton(investText, investButton);
    centerTextOnButton(spyActionText, spyActionButton);
    centerTextOnButton(exitGameText, exitGameButton);
    centerTextOnButton(blockButtonText, blockButton);
    centerTextOnButton(skipBlockButtonText, skipBlockButton);

    bool actionPerformedThisClick = false;
    errorPopupBackground.setFillColor(sf::Color(70, 70, 70, 230));
    errorPopupBackground.setOutlineColor(sf::Color::Red);
    errorPopupBackground.setOutlineThickness(2.f);

    addGameLogEntry("Welcome to Coup!");

    Player* currentPlayer = nullptr;
    std::string selectingTargetFor = "";
    bool displayActionButtons = true;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Dismiss error popup on click
            if (displayErrorPopup && event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                displayErrorPopup = false;
            }

            if (currentState == ENTERING_PLAYERS) {
                // Input handling for player names
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode < 128) {
                        if (event.text.unicode == '\n' || event.text.unicode == '\r') {
                            if (!currentPlayerName.empty()) {
                                enteredPlayers.push_back(currentPlayerName);
                                currentPlayerName = "";
                            }
                        } else if (event.text.unicode == '\b') {
                            if (!currentPlayerName.empty()) {
                                currentPlayerName.pop_back();
                            }
                        } else {
                            currentPlayerName += static_cast<char>(event.text.unicode);
                        }
                    }
                }

                // Button clicks
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    if (enterPlayerButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (!currentPlayerName.empty()) {
                            enteredPlayers.push_back(currentPlayerName);
                            currentPlayerName = "";
                        }
                    }
                    if (startGameButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (enteredPlayers.size() >= 2) {
                            try {
                                game.initializeGame(enteredPlayers);
                                currentPlayer = game.getCurrentPlayer();
                                currentState = PLAYING;
                                addGameLogEntry("Game started with " + std::to_string(game.getAllPlayers().size()) + " players.");
                                addGameLogEntry(currentPlayer->getName() + "'s turn.");

                                // Initialize playerBoxes
                                float currentBoxX = 50.f;
                                float currentBoxY = 50.f;
                                float boxSpacingX = 170.f;
                                float boxSpacingY = 150.f;
                                int boxesPerRow = 3;
                                size_t i = 0;
                                for (Player* p : game.getAllPlayers()) {
                                    sf::RectangleShape playerRect(sf::Vector2f(150, 100));
                                    playerRect.setPosition(currentBoxX + (i % boxesPerRow) * boxSpacingX,
                                                          currentBoxY + (i / boxesPerRow) * boxSpacingY);
                                    playerRect.setFillColor(sf::Color(50, 50, 50));
                                    playerRect.setOutlineColor(sf::Color::White);
                                    playerRect.setOutlineThickness(2.f);
                                    playerBoxes[p->getName()] = playerRect;
                                    i++;
                                }
                            } catch (const std::exception& e) {
                                triggerErrorPopup("Error starting game: " + std::string(e.what()), font);
                            }
                        } else {
                            triggerErrorPopup("Need at least 2 players to start the game!", font);
                        }
                    }
                }
            } else if (currentState == PLAYING) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    // Gather Button
                    if (gatherButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (currentPlayer->isSanctioned()) {
                            triggerErrorPopup(currentPlayer->getName() + " is sanctioned and cannot gather coins.", font);
                        } else {
                            addGameLogEntry(currentPlayer->getName() + " gathered 1 coin.");
                            currentPlayer->gather(game);
                            if (!blockPending) {
                                game.nextTurn();
                                currentPlayer = game.getCurrentPlayer();
                                displayActionButtons = true;
                                addGameLogEntry(currentPlayer->getName() + "'s turn.");
                            }
                        }
                    }
                    // Tax Button
                    else if (taxButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (currentPlayer->isSanctioned()) {
                            triggerErrorPopup(currentPlayer->getName() + " is sanctioned and cannot tax.", font);
                        } else {
                            int taxAmount = currentPlayer->tax(game);
                            addGameLogEntry(currentPlayer->getName() + " performs Tax, gaining " + std::to_string(taxAmount) + " coins.");
                            game.recordAction(currentPlayer, "tax", nullptr);

                            Player* blocker = game.tryBlock("tax", currentPlayer, nullptr);
                            if (blocker) {
                                blockPending = true;
                                actionPerformer = currentPlayer;
                                actionTypeToBlock = "tax";
                                potentialBlocker = blocker;
                                blockCost = 0;
                                addGameLogEntry(potentialBlocker->getName() + " (as " + potentialBlocker->role() + ") can block " + actionPerformer->getName() + "'s " + actionTypeToBlock + "!");
                                displayActionButtons = false;
                            } else {
                                currentPlayer->setCoins(currentPlayer->getCoins() + taxAmount);
                                game.nextTurn();
                                currentPlayer = game.getCurrentPlayer();
                                displayActionButtons = true;
                                addGameLogEntry("Tax was not blocked. " + currentPlayer->getName() + "'s turn.");
                            }
                        }
                    }
                    // Bribe Button
                    else if (bribeButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (currentPlayer->getCoins() < 4) {
                            triggerErrorPopup(currentPlayer->getName() + " does not have enough coins to bribe (needs 4).", font);
                        } else {
                            addGameLogEntry(currentPlayer->getName() + " performs Bribe, paying 4 coins.");
                            currentPlayer->bribe(game);
                            game.recordAction(currentPlayer, "bribe", nullptr);

                            Player* blocker = game.tryBlock("bribe", currentPlayer, nullptr);
                            if (blocker) {
                                blockPending = true;
                                actionPerformer = currentPlayer;
                                actionTypeToBlock = "bribe";
                                potentialBlocker = blocker;
                                blockCost = 0;
                                addGameLogEntry(potentialBlocker->getName() + " (as " + potentialBlocker->role() + ") can block " + actionPerformer->getName() + "'s " + actionTypeToBlock + "!");
                                displayActionButtons = false;
                            } else {
                                game.nextTurn();
                                currentPlayer = game.getCurrentPlayer();
                                displayActionButtons = true;
                                addGameLogEntry("Bribe was not blocked. " + currentPlayer->getName() + "'s turn.");
                            }
                        }
                    }
                    // Arrest Button
                    else if (arrestButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (currentPlayer->getCoins() < 3) {
                            triggerErrorPopup(currentPlayer->getName() + " does not have enough coins to arrest (needs 3).", font);
                        } else {
                            selectingTargetFor = "arrest";
                            addGameLogEntry(currentPlayer->getName() + " is choosing a target for Arrest.");
                        }
                    }
                    // Sanction Button
                    else if (sanctionButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (currentPlayer->getCoins() < 3) {
                            triggerErrorPopup(currentPlayer->getName() + " does not have enough coins to sanction (needs 3).", font);
                        } else {
                            selectingTargetFor = "sanction";
                            addGameLogEntry(currentPlayer->getName() + " is choosing a target for Sanction.");
                        }
                    }
                    // Coup Button
                    else if (coupButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (currentPlayer->getCoins() < 7) {
                            triggerErrorPopup(currentPlayer->getName() + " does not have enough coins to coup (needs 7).", font);
                        } else {
                            selectingTargetFor = "coup";
                            addGameLogEntry(currentPlayer->getName() + " is choosing a target for Coup.");
                        }
                    }
                    // Invest Button (Baron)
                    else if (currentPlayer->role() == "Baron" && investButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        Baron* baronPlayer = dynamic_cast<Baron*>(currentPlayer);
                        if (baronPlayer) {
                            baronPlayer->invest();
                            addGameLogEntry(baronPlayer->getName() + " (Baron) performed Invest, gaining 6 coins.");
                            if (!blockPending) {
                                game.nextTurn();
                                currentPlayer = game.getCurrentPlayer();
                                displayActionButtons = true;
                                addGameLogEntry(currentPlayer->getName() + "'s turn.");
                            }
                        } else {
                            triggerErrorPopup("Error: Could not cast current player to Baron.", font);
                        }
                    }
                    // Spy Action Button
                    else if (currentPlayer->role() == "Spy" && spyActionButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        selectingTargetFor = "spy_action";
                        addGameLogEntry(currentPlayer->getName() + " (Spy) is choosing a target for Spy Action.");
                    }
                }

                // Target selection logic
                if (selectingTargetFor != "") {
                    for (Player* p : game.getAllPlayers()) {
                        if (p->isAlive() && p != currentPlayer) {
                            if (playerBoxes.find(p->getName()) == playerBoxes.end()) {
                                std::cerr << "Error: No rectangle for player " << p->getName() << std::endl;
                                continue;
                            }
                            sf::RectangleShape targetRect = playerBoxes[p->getName()];
                            if (targetRect.getGlobalBounds().contains(mousePos.x, mousePos.y) && event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                                Player* targetPlayer = p;
                                if (selectingTargetFor == "arrest") {
                                    addGameLogEntry(currentPlayer->getName() + " performs Arrest on " + targetPlayer->getName() + ", paying 3 coins.");
                                    currentPlayer->arrest(targetPlayer, game);
                                    game.recordAction(currentPlayer, "arrest", targetPlayer);
                                    if (!blockPending) {
                                        game.nextTurn();
                                        currentPlayer = game.getCurrentPlayer();
                                        displayActionButtons = true;
                                        addGameLogEntry(currentPlayer->getName() + "'s turn.");
                                    }
                                } else if (selectingTargetFor == "sanction") {
                                    addGameLogEntry(currentPlayer->getName() + " performs Sanction on " + targetPlayer->getName() + ", paying 3 coins.");
                                    currentPlayer->sanction(targetPlayer, game);
                                    game.recordAction(currentPlayer, "sanction", targetPlayer);
                                    if (!blockPending) {
                                        game.nextTurn();
                                        currentPlayer = game.getCurrentPlayer();
                                        displayActionButtons = true;
                                        addGameLogEntry(currentPlayer->getName() + "'s turn.");
                                    }
                                } else if (selectingTargetFor == "coup") {
                                    addGameLogEntry(currentPlayer->getName() + " performs Coup on " + targetPlayer->getName() + ", paying 7 coins.");
                                    bool coupSuccessful = currentPlayer->coup(targetPlayer, game);
                                    if (coupSuccessful) {
                                        game.recordAction(currentPlayer, "coup", targetPlayer);
                                        Player* blocker = game.tryBlock("coup", currentPlayer, targetPlayer);
                                        if (blocker) {
                                            blockPending = true;
                                            actionPerformer = currentPlayer;
                                            actionTarget = targetPlayer;
                                            actionTypeToBlock = "coup";
                                            potentialBlocker = blocker;
                                            blockCost = 5;
                                            addGameLogEntry(potentialBlocker->getName() + " (as " + potentialBlocker->role() + ") can block " + actionPerformer->getName() + "'s " + actionTypeToBlock + " on " + actionTarget->getName() + " for " + std::to_string(blockCost) + " coins!");
                                            displayActionButtons = false;
                                        } else {
                                            game.nextTurn();
                                            currentPlayer = game.getCurrentPlayer();
                                            displayActionButtons = true;
                                            addGameLogEntry("Coup was not blocked. " + currentPlayer->getName() + "'s turn.");
                                        }
                                    }
                                } else if (selectingTargetFor == "spy_action") {
                                    if (currentPlayer->role() == "Spy") {
                                        Spy* spyPlayer = dynamic_cast<Spy*>(currentPlayer);
                                        if (spyPlayer) {
                                            spyPlayer->preventArrest(*targetPlayer);
                                            addGameLogEntry(spyPlayer->getName() + " (Spy) used Prevent Arrest on " + targetPlayer->getName() + ".");
                                            game.recordAction(spyPlayer, "prevent_arrest", targetPlayer);
                                            selectingTargetFor = "";
                                            displayActionButtons = true;
                                            addGameLogEntry(currentPlayer->getName() + "'s turn continues (Spy action does not consume turn).");
                                        } else {
                                            triggerErrorPopup("Error: Could not cast current player to Spy.", font);
                                            selectingTargetFor = "";
                                            displayActionButtons = true;
                                        }
                                    } else {
                                        selectingTargetFor = "";
                                        displayActionButtons = true;
                                    }
                                }
                                selectingTargetFor = "";
                                break;
                            }
                        }
                    }
                }
            } else if (blockPending) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    if (blockButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        if (blockCost > 0 && potentialBlocker->getCoins() < blockCost) {
                            triggerErrorPopup(potentialBlocker->getName() + " does not have enough coins to block (" + std::to_string(blockCost) + " needed).", font);
                        } else {
                            addGameLogEntry(potentialBlocker->getName() + " blocked " + actionPerformer->getName() + "'s " + actionTypeToBlock + ".");
                            if (blockCost > 0) {
                                potentialBlocker->setCoins(potentialBlocker->getCoins() - blockCost);
                                addGameLogEntry(potentialBlocker->getName() + " paid " + std::to_string(blockCost) + " coins to block.");
                            }
                            blockPending = false;
                            actionPerformer = nullptr;
                            actionTarget = nullptr;
                            actionTypeToBlock = "";
                            potentialBlocker = nullptr;
                            blockCost = 0;
                            game.nextTurn();
                            currentPlayer = game.getCurrentPlayer();
                            displayActionButtons = true;
                            addGameLogEntry(currentPlayer->getName() + "'s turn.");
                        }
                    } else if (skipBlockButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        addGameLogEntry(potentialBlocker->getName() + " chose NOT to block " + actionPerformer->getName() + "'s " + actionTypeToBlock + ".");
                        if (actionTypeToBlock == "tax") {
                            actionPerformer->setCoins(actionPerformer->getCoins() + game.getLastTaxAmount());
                            addGameLogEntry(actionPerformer->getName() + " received " + std::to_string(game.getLastTaxAmount()) + " coins from Tax.");
                        }
                        blockPending = false;
                        actionPerformer = nullptr;
                        actionTarget = nullptr;
                        actionTypeToBlock = "";
                        potentialBlocker = nullptr;
                        blockCost = 0;
                        game.nextTurn();
                        currentPlayer = game.getCurrentPlayer();
                        displayActionButtons = true;
                        addGameLogEntry(currentPlayer->getName() + "'s turn.");
                    }
                }
            } else if (currentState == GAME_OVER) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                    if (exitGameButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        window.close();
                    }
                }
            }
        }

        // Drawing
        window.clear();
        std::cout << "Rendering state: " << currentState << std::endl; // Debug output

        if (currentState == ENTERING_PLAYERS) {
            std::cout << "Rendering ENTERING_PLAYERS state" << std::endl;
            sf::Text instructionsText = createText("Enter player names (press Enter after each) and click 'Start Game':", font, 20, 50, 50);
            window.draw(instructionsText);

            sf::Text currentInputText = createText("Current Player Name: " + currentPlayerName, font, 18, 50, 100);
            window.draw(currentInputText);

            sf::Text enteredPlayersText = createText("Players:", font, 18, 50, 150);
            window.draw(enteredPlayersText);

            float currentY = 180;
            for (const std::string& name : enteredPlayers) {
                sf::Text pText = createText("- " + name, font, 16, 70, currentY);
                window.draw(pText);
                currentY += 25;
            }

            window.draw(enterPlayerButton);
            window.draw(enterPlayerText);
            window.draw(startGameButton);
            window.draw(startGameText);
        } else if (currentState == PLAYING || blockPending) {
            // Check for game over
            int alivePlayers = 0;
            for (Player* p : game.getAllPlayers()) {
                if (p->isAlive()) {
                    alivePlayers++;
                }
            }
            if (alivePlayers <= 1) {
                currentState = GAME_OVER;
            } else {
                // Draw players
                for (Player* p : game.getAllPlayers()) {
                    if (playerBoxes.find(p->getName()) == playerBoxes.end()) {
                        std::cerr << "Error: No rectangle for player " << p->getName() << std::endl;
                        continue;
                    }
                    sf::RectangleShape playerRect = playerBoxes[p->getName()];
                    if (p == currentPlayer) {
                        playerRect.setOutlineColor(sf::Color::Yellow);
                        playerRect.setOutlineThickness(4.f);
                    } else if (p == potentialBlocker && blockPending) {
                        playerRect.setOutlineColor(sf::Color::Magenta);
                        playerRect.setOutlineThickness(4.f);
                    } else {
                        playerRect.setOutlineColor(sf::Color::White);
                        playerRect.setOutlineThickness(2.f);
                    }
                    if (!p->isAlive()) {
                        playerRect.setFillColor(sf::Color(20, 20, 20));
                    } else {
                        playerRect.setFillColor(sf::Color(50, 50, 50));
                    }
                    window.draw(playerRect);

                    sf::Text nameText = createText(p->getName(), font, 18, playerRect.getPosition().x + 5, playerRect.getPosition().y + 5);
                    window.draw(nameText);

                    std::string status = "Coins: " + std::to_string(p->getCoins());
                    if (!p->isAlive()) {
                        status += "\nELIMINATED";
                    }
                    if (p->isSanctioned()) {
                        status += "\n(SANCTIONED)";
                    }
                    status += "\nRole: " + p->role();
                    sf::Text infoText = createText(status, font, 14, playerRect.getPosition().x + 5, playerRect.getPosition().y + 30);
                    window.draw(infoText);
                }

                // Draw action buttons
                if (displayActionButtons && !blockPending) {
                    window.draw(gatherButton);
                    window.draw(gatherText);
                    window.draw(taxButton);
                    window.draw(taxText);
                    window.draw(bribeButton);
                    window.draw(bribeText);
                    window.draw(arrestButton);
                    window.draw(arrestText);
                    window.draw(sanctionButton);
                    window.draw(sanctionText);
                    window.draw(coupButton);
                    window.draw(coupText);

                    if (currentPlayer && currentPlayer->role() == "Baron") {
                        window.draw(investButton);
                        window.draw(investText);
                    }
                    if (currentPlayer && currentPlayer->role() == "Spy") {
                        window.draw(spyActionButton);
                        window.draw(spyActionText);
                    }
                }

                // Draw target selection prompt
                if (selectingTargetFor != "") {
                    sf::Text targetPromptText = createText("Select a target for " + selectingTargetFor + ":", font, 20, (window.getSize().x - 400) / 2, 450);
                    targetPromptText.setFillColor(sf::Color::Yellow);
                    window.draw(targetPromptText);
                }

                // Draw blocking prompt
                if (blockPending) {
                    std::string promptMessage = potentialBlocker->getName() + " (as " + potentialBlocker->role() + "), do you want to block " + actionPerformer->getName() + "'s " + actionTypeToBlock;
                    if (actionTarget) {
                        promptMessage += " on " + actionTarget->getName();
                    }
                    promptMessage += "?";
                    if (blockCost > 0) {
                        promptMessage += " (Costs " + std::to_string(blockCost) + " coins)";
                    }
                    blockPromptText = createText(promptMessage, font, 20, (window.getSize().x - 600) / 2, window.getSize().y / 2 - 100);
                    blockPromptText.setFillColor(sf::Color::Yellow);
                    window.draw(blockPromptText);
                    window.draw(blockButton);
                    window.draw(blockButtonText);
                    window.draw(skipBlockButton);
                    window.draw(skipBlockButtonText);
                }
            }
        } else if (currentState == GAME_OVER) {
            sf::Text gameOverText = createText("GAME OVER!", font, 40, (window.getSize().x - 200) / 2, window.getSize().y / 2 - 50);
            gameOverText.setFillColor(sf::Color::Red);
            window.draw(gameOverText);

            Player* winner = nullptr;
            for (Player* p : game.getAllPlayers()) {
                if (p->isAlive()) {
                    winner = p;
                    break;
                }
            }
            if (winner) {
                sf::Text winnerText = createText(winner->getName() + " wins!", font, 30, (window.getSize().x - 200) / 2, window.getSize().y / 2 + 20);
                winnerText.setFillColor(sf::Color::Green);
                window.draw(winnerText);
            }

            window.draw(exitGameButton);
            window.draw(exitGameText);
        }

        // Draw Game Log
        sf::RectangleShape logPanel(sf::Vector2f(200, MAX_LOG_MESSAGES * LOG_ENTRY_HEIGHT + 20));
        logPanel.setFillColor(sf::Color(30, 30, 30, 180));
        logPanel.setPosition(LOG_PANEL_X, 10.f);
        window.draw(logPanel);

        sf::Text logTitle = createText("Game Log:", font, 16, LOG_PANEL_X + 5, 15.f);
        logTitle.setFillColor(sf::Color::Cyan);
        window.draw(logTitle);

        float currentLogY = 10.f + LOG_ENTRY_HEIGHT + 10;
        for (const std::string& entry : gameLog) {
            sf::Text logTextElementProto = createText(entry, font, LOG_FONT_SIZE, LOG_PANEL_X + 5, currentLogY);
            logTextElementProto.setFillColor(sf::Color::White);
            window.draw(logTextElementProto);
            currentLogY += LOG_ENTRY_HEIGHT;
            if (currentLogY > window.getSize().y - LOG_ENTRY_HEIGHT) break;
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