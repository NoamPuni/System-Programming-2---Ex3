#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <deque> // Added for game log
#include "Game.hpp"

// Helper function to create text elements (remains unchanged)
sf::Text createText(const std::string& content, const sf::Font& font, unsigned int size, float x, float y) {
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::White);
    text.setPosition(x, y);
    return text;
}

// NEW: Game Log variables
std::deque<std::string> gameLog;
const unsigned int MAX_LOG_MESSAGES = 15; // Display up to 15 messages
const float LOG_PANEL_X = 580.f;    // X position for the log panel (adjusted for 800px width)
const float LOG_ENTRY_HEIGHT = 18.f; // Height per log entry (font size 14 + padding)
const unsigned int LOG_FONT_SIZE = 14;

// NEW: Error Popup variables
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
    // Text and background properties will be set in the drawing phase
    // to correctly calculate size based on message length.
}


int main() {
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        // This error is critical and happens before GUI is fully usable for popups,
        // so it remains on std::cerr.
        std::cerr << "Failed to load font!" << std::endl;
        return 1;
    }

    Game game;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Coup Game - GUI");
    window.setFramerateLimit(60); // Optional: for smoother experience

    // Game states
    enum GameState { ENTERING_PLAYERS, PLAYING, SELECTING_ARREST_TARGET };
    GameState currentState = ENTERING_PLAYERS;

    // Player entry variables
    std::vector<std::string> enteredPlayers;
    std::string currentPlayerName = "";
    bool gameInitialized = false;

    // Target selection variables
    std::vector<sf::RectangleShape> targetButtons;
    std::vector<sf::Text> targetTexts;

    // Action buttons (for playing state)
    sf::RectangleShape gatherButton(sf::Vector2f(120, 40));
    gatherButton.setFillColor(sf::Color::Blue);
    gatherButton.setPosition(50, 500);

    sf::RectangleShape taxButton(sf::Vector2f(120, 40));
    taxButton.setFillColor(sf::Color::Green);
    taxButton.setPosition(200, 500);

    sf::RectangleShape bribeButton(sf::Vector2f(120, 40));
    bribeButton.setFillColor(sf::Color::Magenta);
    bribeButton.setPosition(350, 500);

    sf::RectangleShape arrestButton(sf::Vector2f(120, 40));
    arrestButton.setFillColor(sf::Color::Red);
    arrestButton.setPosition(500, 500); // Ends at x = 620

    // Player entry buttons
    sf::RectangleShape enterPlayerButton(sf::Vector2f(150, 40));
    enterPlayerButton.setFillColor(sf::Color::Cyan);
    enterPlayerButton.setPosition(50, 400);

    sf::RectangleShape startGameButton(sf::Vector2f(150, 40));
    startGameButton.setFillColor(sf::Color::Yellow);
    startGameButton.setPosition(250, 400);

    sf::Text gatherText = createText("Gather", font, 18, 65, 510);
    sf::Text taxText = createText("Tax", font, 18, 235, 510);
    sf::Text bribeText = createText("Bribe (-4)", font, 16, 360, 510);
    sf::Text arrestText = createText("Arrest", font, 18, 525, 510); // Text for arrest button
    sf::Text enterPlayerText = createText("Enter Player", font, 18, 75, 410);
    sf::Text startGameText = createText("Start Game", font, 18, 285, 410);

    bool actionPerformedThisClick = false;

    // Configure Error Popup Background Style
    errorPopupBackground.setFillColor(sf::Color(70, 70, 70, 230)); // Dark semi-transparent
    errorPopupBackground.setOutlineColor(sf::Color::Red);
    errorPopupBackground.setOutlineThickness(2.f);

    addGameLogEntry("Welcome to Coup! Enter player names to begin.");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            // Handle input for dismissing error popup first
            if (displayErrorPopup) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    displayErrorPopup = false;
                    actionPerformedThisClick = true; // Consume this click
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    displayErrorPopup = false;
                    actionPerformedThisClick = true; // Consume this key press
                }
                // If popup was dismissed, skip other event handling for this specific event trigger
                if (actionPerformedThisClick && 
                    (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::KeyPressed)) {
                    // Reset actionPerformedThisClick for the *next* event if it was a release
                    if(event.type == sf::Event::MouseButtonReleased) actionPerformedThisClick = false;
                    continue; 
                }
            }


            if (currentState == ENTERING_PLAYERS) {
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode < 128) {
                        if (event.text.unicode == '\b') { // Backspace
                            if (!currentPlayerName.empty()) {
                                currentPlayerName.pop_back();
                            }
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
                            actionPerformedThisClick = true;
                        } else if (currentPlayerName.empty()) {
                             addGameLogEntry("Cannot add empty player name.");
                        } else {
                             addGameLogEntry("Maximum players (6) reached.");
                        }
                    }
                    else if (startGameButton.getGlobalBounds().contains(mousePos)) {
                        if (enteredPlayers.size() >= 2) {
                            try {
                                game.initializeGame(enteredPlayers);
                                gameInitialized = true;
                                currentState = PLAYING;
                                addGameLogEntry("Game started with " + std::to_string(enteredPlayers.size()) + " players!");
                                actionPerformedThisClick = true;
                            } catch (const std::exception& e) {
                                triggerErrorPopup("Failed to initialize game: " + std::string(e.what()), font);
                                actionPerformedThisClick = true;
                            }
                        } else {
                            addGameLogEntry("Need at least 2 players to start.");
                        }
                    }
                }
            }
            else if (currentState == PLAYING) {
                if (event.type == sf::Event::MouseButtonPressed && !actionPerformedThisClick) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    Player* currentPlayer = game.getCurrentPlayer();

                    if (currentPlayer != nullptr) {
                        try {
                            if (gatherButton.getGlobalBounds().contains(mousePos)) {
                                currentPlayer->gather();
                                addGameLogEntry(currentPlayer->getName() + " gathered 1 coin.");
                                game.nextTurn();
                                actionPerformedThisClick = true;
                            }
                            else if (taxButton.getGlobalBounds().contains(mousePos)) {
                                currentPlayer->tax(game);
                                addGameLogEntry(currentPlayer->getName() + " performed tax.");
                                game.nextTurn();
                                actionPerformedThisClick = true;
                            }
                            else if (bribeButton.getGlobalBounds().contains(mousePos)) {
                                currentPlayer->bribe(game);
                                addGameLogEntry(currentPlayer->getName() + " performed bribe."); // Specifics in Player.cpp logic
                                game.nextTurn();
                                actionPerformedThisClick = true;
                            }
                            else if (arrestButton.getGlobalBounds().contains(mousePos)) {
                                currentState = SELECTING_ARREST_TARGET;
                                targetButtons.clear();
                                targetTexts.clear();
                                auto playersInfo = game.getPlayersWithRoles();
                                float yPos = 200;
                                for (const auto& playerInfo : playersInfo) {
                                    if (playerInfo.first != currentPlayer->getName()) {
                                        sf::RectangleShape targetButton(sf::Vector2f(200, 40));
                                        targetButton.setFillColor(sf::Color::Yellow);
                                        targetButton.setPosition(window.getSize().x / 2.f - targetButton.getSize().x / 2.f, yPos);
                                        targetButtons.push_back(targetButton);
                                        sf::Text targetText = createText(playerInfo.first, font, 18, targetButton.getPosition().x + 20, yPos + 10);
                                        targetTexts.push_back(targetText);
                                        yPos += 50;
                                    }
                                }
                                if (targetButtons.empty()) {
                                    addGameLogEntry("No other players available for arrest!");
                                    currentState = PLAYING;
                                } else {
                                    addGameLogEntry(currentPlayer->getName() + " is selecting arrest target...");
                                }
                                actionPerformedThisClick = true;
                            }
                        } catch (const std::exception& e) {
                            triggerErrorPopup("Action Error: " + std::string(e.what()), font);
                            actionPerformedThisClick = true;
                        }
                    }
                }
            }
            else if (currentState == SELECTING_ARREST_TARGET) {
                if (event.type == sf::Event::MouseButtonPressed && !actionPerformedThisClick) {
                    sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
                    Player* currentPlayer = game.getCurrentPlayer();

                    if (currentPlayer != nullptr) {
                        bool targetClicked = false;
                        for (size_t i = 0; i < targetButtons.size(); ++i) {
                            if (targetButtons[i].getGlobalBounds().contains(mousePos)) {
                                std::string targetName = targetTexts[i].getString();
                                targetClicked = true;
                                try {
                                    Player* targetPlayer = nullptr;
                                    auto allPlayers = game.getAllPlayers(); //
                                    for (Player* player : allPlayers) {
                                        if (player->getName() == targetName) {
                                            targetPlayer = player;
                                            break;
                                        }
                                    }
                                    if (targetPlayer != nullptr) {
                                        bool arrestSuccessful = currentPlayer->arrest(targetPlayer, game); //
                                        if (arrestSuccessful) {
                                            addGameLogEntry(currentPlayer->getName() + " arrested " + targetName + "!");
                                            game.nextTurn();
                                        } else {
                                            // The Player::arrest method should throw an exception or return detailed error
                                            // For now, using a generic message that might be improved with more info from game logic
                                            triggerErrorPopup("Arrest failed. Target: " + targetName + ". (May be invalid or protected)", font);
                                        }
                                    } else {
                                        triggerErrorPopup("Target player " + targetName + " not found.", font);
                                    }
                                } catch (const std::exception& e) {
                                    triggerErrorPopup("Arrest Error: " + std::string(e.what()), font);
                                }
                                currentState = PLAYING;
                                actionPerformedThisClick = true;
                                break; 
                            }
                        }
                         if (!targetClicked) { // Click was not on a target button
                            // Potentially add a way to cancel by clicking outside buttons, or rely on ESC
                        }
                    }
                }
                
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        currentState = PLAYING;
                        addGameLogEntry("Arrest action cancelled.");
                        actionPerformedThisClick = true;
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                actionPerformedThisClick = false;
            }
        }

        window.clear(sf::Color(30, 30, 30)); // Dark background for the game

        // === Draw game elements based on state ===
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
                sf::Text playerText = createText(playerDisplay, font, 18, 70, 190 + i * 25);
                window.draw(playerText);
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
        else if (currentState == PLAYING || currentState == SELECTING_ARREST_TARGET) {
            std::string turnTitle = "Turn: " + game.turn();
            if(currentState == SELECTING_ARREST_TARGET) turnTitle += " - Select Arrest Target (ESC to cancel)";
            sf::Text turnText = createText(turnTitle, font, 24, 20, 20); // Adjusted X for more space
            window.draw(turnText);

            float yPlayerInfo = 70; // Start Y for player info
            auto playersInfo = game.getPlayersWithRoles(); //
            Player* gameCurrentPlayer = game.getCurrentPlayer(); //

            for (const auto& playerInfo : playersInfo) {
                std::ostringstream oss;
                Player* p = nullptr; // Find player object to get coins
                for(auto pl_ptr : game.getAllPlayers()){ //
                    if(pl_ptr->getName() == playerInfo.first){
                        p = pl_ptr;
                        break;
                    }
                }

                oss << playerInfo.first;
                if (p && p->isAlive()) { //
                     oss << " (" << playerInfo.second << ")"; // Show role only if alive
                    if (p == gameCurrentPlayer || !p->isAlive()) { // Show coins for current player or if revealed (eliminated)
                         oss << " - Coins: " << p->getCoins(); //
                    } else {
                        // For other alive players, you might want to hide their exact role or coin count
                        // For this version, we show role, but coins could be '?'
                        oss << " - Coins: " << p->getCoins(); // Or '?' if rules dictate hiding coins
                    }
                    if (!p->isAlive()) oss << " (Eliminated)"; //

                } else if (p && !p->isAlive()){
                     oss << " (" << playerInfo.second << ") - Coins: " << p->getCoins() << " (Eliminated)"; //
                }
                else {
                     oss << " (Error: Player data not found)";
                }


                sf::Text playerText = createText(oss.str(), font, 18, 20, yPlayerInfo);
                if (p == gameCurrentPlayer && p->isAlive()) { //
                    playerText.setFillColor(sf::Color::Yellow); // Highlight current player
                } else if (p && !p->isAlive()){ //
                    playerText.setFillColor(sf::Color(128,128,128)); // Grey out eliminated players
                }
                window.draw(playerText);
                yPlayerInfo += 30;
            }

            if (currentState == PLAYING) {
                window.draw(gatherButton); window.draw(gatherText);
                window.draw(taxButton); window.draw(taxText);
                window.draw(bribeButton); window.draw(bribeText);
                window.draw(arrestButton); window.draw(arrestText);
                sf::Text instructionText = createText("Choose an action:", font, 18, 50, 460);
                window.draw(instructionText);
            } else { // SELECTING_ARREST_TARGET
                sf::Text instructionText = createText("Click on a player to arrest.", font, 16, 50, window.getSize().y - 50.f);
                 window.draw(instructionText);
                for (size_t i = 0; i < targetButtons.size(); ++i) {
                    window.draw(targetButtons[i]);
                    window.draw(targetTexts[i]);
                }
            }
        }

        // NEW: Draw Game Log (on the right side)
        float currentLogY = 20.f;
        sf::Text logTextElementProto; // Prototype for log text
        logTextElementProto.setFont(font);
        logTextElementProto.setCharacterSize(LOG_FONT_SIZE);
        logTextElementProto.setFillColor(sf::Color(200, 200, 200)); // Light grey for log

        // Optional: Background for log area
        sf::RectangleShape logAreaBg(sf::Vector2f(window.getSize().x - LOG_PANEL_X, window.getSize().y));
        logAreaBg.setPosition(LOG_PANEL_X, 0);
        logAreaBg.setFillColor(sf::Color(40, 40, 40, 150)); // Slightly darker, semi-transparent
        window.draw(logAreaBg);


        for (const auto& message : gameLog) {
            logTextElementProto.setString(message);
            logTextElementProto.setPosition(LOG_PANEL_X + 5.f, currentLogY); // Small padding from left of log panel
            window.draw(logTextElementProto);
            currentLogY += LOG_ENTRY_HEIGHT;
        }


        // NEW: Draw Error Popup (on top of everything else if active)
        if (displayErrorPopup) {
            errorPopupTextElement.setFont(font);
            errorPopupTextElement.setString(currentErrorPopupMessage);
            errorPopupTextElement.setCharacterSize(ERROR_FONT_SIZE);
            errorPopupTextElement.setFillColor(sf::Color::White);

            // Calculate background size based on text, with wrapping awareness (simple version)
            // This simple version doesn't truly wrap, but tries to make background reasonable.
            // For real wrapping, you'd need to break the string into multiple sf::Text objects.
            sf::FloatRect textBounds = errorPopupTextElement.getLocalBounds();
            float maxWidth = window.getSize().x * 0.8f;
            float textWidth = textBounds.width;
            if (textWidth > maxWidth) textWidth = maxWidth; // Limit width

            float padding = 25.f;
            errorPopupBackground.setSize(sf::Vector2f(textWidth + 2 * padding, textBounds.height + 2 * padding + 15));
            errorPopupBackground.setOrigin(errorPopupBackground.getSize().x / 2.f, errorPopupBackground.getSize().y / 2.f);
            errorPopupBackground.setPosition(window.getSize().x / 2.f, window.getSize().y / 2.f);
            
            // Center text on the background
            // For long text that we "limited" in width for background, actual text might still overflow.
            // Proper wrapping is needed for perfect visual, but this positions the existing text object.
            errorPopupTextElement.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
            errorPopupTextElement.setPosition(errorPopupBackground.getPosition());
            
            window.draw(errorPopupBackground);
            window.draw(errorPopupTextElement);
        }

        window.display();
    }

    return 0;
}