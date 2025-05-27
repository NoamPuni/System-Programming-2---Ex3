#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>

// Helper function to create text for display
sf::Text createText(const std::string& str, const sf::Font& font, unsigned int charSize, const sf::Color& color, float x, float y) {
    sf::Text text;
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(charSize);
    text.setFillColor(color);
    text.setPosition(x, y);
    return text;
}

// A simple "Button" class for demonstration
class Button {
public:
    sf::RectangleShape shape;
    sf::Text text;
    bool isHovered;

    Button(const std::string& label, const sf::Font& font, sf::Vector2f size, sf::Vector2f position) {
        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(sf::Color(100, 100, 100)); // Dark grey

        text.setFont(font);
        text.setString(label);
        text.setCharacterSize(20);
        text.setFillColor(sf::Color::White);
        centerText(); // Center text within the button shape

        isHovered = false;
    }

    void update(sf::Vector2i mousePos) {
        if (shape.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
            if (!isHovered) {
                shape.setFillColor(sf::Color(150, 150, 150)); // Lighter grey on hover
                isHovered = true;
            }
        } else {
            if (isHovered) {
                shape.setFillColor(sf::Color(100, 100, 100)); // Back to dark grey
                isHovered = false;
            }
        }
    }

    bool isClicked(sf::Vector2i mousePos) {
        return isHovered && sf::Mouse::isButtonPressed(sf::Mouse::Left);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
        window.draw(text);
    }

private:
    void centerText() {
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
        text.setPosition(shape.getPosition() + sf::Vector2f(shape.getSize().x / 2.0f, shape.getSize().y / 2.0f));
    }
};


int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Game GUI");

    // Load a font (important for displaying text)
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        // Handle error, e.g., print to console and exit
        return -1;
    }

    int currentPlayerTurn = 1;

    // Game state text
    sf::Text statusText = createText("Welcome! Player 1's turn.", font, 24, sf::Color::White, 50, 50);

    // Example button
    Button nextTurnButton("Next Turn", font, sf::Vector2f(150, 50), sf::Vector2f(325, 200));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseMoved) {
                nextTurnButton.update(sf::Mouse::getPosition(window));
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    if (nextTurnButton.isHovered) { // Check if the button was hovered when clicked
                        // Game logic for next turn
                        currentPlayerTurn = (currentPlayerTurn == 1) ? 2 : 1;
                        statusText.setString("Player " + std::to_string(currentPlayerTurn) + "'s turn!");
                    }
                }
            }
        }

        window.clear(sf::Color(50, 50, 50)); // Dark background

        window.draw(statusText);
        nextTurnButton.draw(window);

        window.display();
    }

    return 0;
}