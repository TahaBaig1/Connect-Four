#include "Game.h"

int main() {
	sf::RenderWindow window(sf::VideoMode(850, 850), "Connect 4", sf::Style::Close);
	window.setFramerateLimit(15);
	Game game(window);
	game.run();
	return 0;
}