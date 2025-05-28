#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "Game.hpp"
#include "Merchant.hpp" // נשתמש בסוחר כדוגמה

// פונקציה עוזרת ליצירת טקסט
sf::Text createText(const std::string& content, const sf::Font& font, unsigned int size, float x, float y) {
    sf::Text text;
    text.setFont(font);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(sf::Color::White);
    text.setPosition(x, y);
    return text;
}

int main() {
    // טען פונט (תצטרך קובץ font מתאים במדריך, לדוגמה arial.ttf)
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return 1;
    }

    // יצירת משחק
    Game game;
    Merchant* p1 = new Merchant("Alice");
    Merchant* p2 = new Merchant("Bob");

    game.addPlayer(p1);
    game.addPlayer(p2);

    // יצירת חלון
    sf::RenderWindow window(sf::VideoMode(800, 600), "Coup Game - GUI");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);

        // תור נוכחי
        std::string currentTurn = "Current Turn: " + game.turn();
        sf::Text turnText = createText(currentTurn, font, 24, 50, 20);
        window.draw(turnText);

        // שחקנים פעילים
        std::vector<std::string> activePlayers = game.players();
        float y = 80;
        for (Player* player : {p1, p2}) {
            std::ostringstream oss;
            oss << player->getName() << " - Coins: " << player->getCoins();
            sf::Text playerText = createText(oss.str(), font, 20, 50, y);
            window.draw(playerText);
            y += 40;
        }

        window.display();
    }

    delete p1;
    delete p2;
    return 0;
}
