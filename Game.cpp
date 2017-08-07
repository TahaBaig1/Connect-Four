#include "Game.h"

Game::Game(int numConnected_, int turn_) : numConnected(numConnected_), 
										   turn(turn_), 
										   window(sf::VideoMode(700, 700), "Connect Four", sf::Style::Close),
										   board(window)
{
	//setting up graphics for game marker
	float markerRadius = window.getSize().x / board.getWidth() / 3;
	marker.setRadius(markerRadius);
	marker.setOrigin(markerRadius, markerRadius);
}

void Game::run() {
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
		}

		window.clear();
		board.drawBoard();
		window.display();
	}
}

int Game::checkWon(Position last)
{
	return 0;
}

int Game::convertPixelsToColumn(float x) {
	return 0;
}

void Game::drawMarker() {

}

void Game::animateDrop(Position p) {

}
