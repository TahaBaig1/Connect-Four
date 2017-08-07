#include "Game.h"
#include <iostream>

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
		int currentCol = convertPixelsToColumn(sf::Mouse::getPosition(window));
		if(currentCol != -1) drawMarker(currentCol);
		window.display();
	}
}

int Game::checkWon(Position last)
{
	return 0;
}

//Return the column the mouse is hovering over given the mouse's location in pixels
//If mouse is outside the boundaries of the screen, return -1
int Game::convertPixelsToColumn(sf::Vector2i pos) {
	if (pos.x < 0 || pos.x > window.getSize().x || pos.y < 0 || pos.y > window.getSize().y) return -1;
	int numColumns = board.getWidth();
	float columnLength = window.getSize().x / numColumns;

	return (int) pos.x / columnLength;
}

void Game::drawMarker(int col) {
	if (turn % 2 == 0) marker.setFillColor(board.getColor1());
	else               marker.setFillColor(board.getColor2());

	float xPos = 1.5 * marker.getRadius();
	xPos += col * 3 * marker.getRadius();
	marker.setPosition(xPos, 1.5 * marker.getRadius());
	window.draw(marker);
}

void Game::animateDrop(Position p) {

}
