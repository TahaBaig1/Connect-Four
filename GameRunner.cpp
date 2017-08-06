#include "Board.h"

int main() {
	Board board;
	sf::RenderWindow& window = board.getWindow();

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
		}

		window.clear();
		board.drawBoard();
		window.display();
	}

	return 0;
}
