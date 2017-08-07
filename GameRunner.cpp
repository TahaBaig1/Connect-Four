#include "Game.h"

int main() {
	sf::RenderWindow window(sf::VideoMode(850, 850), "Connect 4", sf::Style::Close);
	Game game(window);
	game.run();
}