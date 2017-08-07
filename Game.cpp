#include "Game.h"
#include <iostream>

Game::Game(int numConnected_, int turn_) : numConnected(numConnected_), 
										   turn(turn_), 
										   window(sf::VideoMode(850, 850), "Connect Four", sf::Style::Close),
										   board(window)
{
	//setting up graphics for game marker
	float markerRadius = window.getSize().x / board.getWidth() / 3;
	marker.setRadius(markerRadius);
	marker.setOrigin(markerRadius, markerRadius);
}

//game loop
//returns true/false depending on if players wishes to play again
bool Game::run() {
	Status gameStatus = CONTINUE;

	//Playing game
	while (window.isOpen() && gameStatus == CONTINUE) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();
			if (event.type == sf::Event::MouseButtonPressed) {
				int col = convertPixelsToColumn(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
				if (col == -1)  break;
				Position placed = board.addPiece(col, getCurrentColor());
				if (placed.x == -1 || placed.y == -1) break;
				gameStatus = checkWon(placed);
				if(gameStatus == CONTINUE) turn++;
			}
		}
		window.clear();

		board.drawBoard();
		int currentCol = convertPixelsToColumn(sf::Mouse::getPosition(window));
		if(currentCol != -1) drawMarker(currentCol);
		window.display();
	}

	//Displaying end game text when game is over
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type = sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) window.close();
				if (event.key.code == sf::Keyboard::Return)  return true;
			}
		}

		window.clear();
		board.drawBoard();
		drawEndText(gameStatus);
		window.display();
	}

	return false;
}

sf::Color Game::getCurrentColor() {
	if (turn % 2 == 0) return board.getColor1();
	else               return board.getColor2();
}

//checks game board and returns curren game status (CONTINUE, TIE, WIN)
//works via passing in last position where a piece was dropped and searching horizontally, vertically, and diagonally from piece to find connections
Status Game::checkWon(Position& placed) {
	if (turn < numConnected) return CONTINUE; //if not enough pieces have been put on the board for a connection, then continue game
	if (turn == board.getWidth() * board.getHeight()) return TIE; //if entire board is filled with no winner, game is tied
     
	Position checkPos = placed;
	sf::CircleShape* pieces = board.getPieces();
	sf::CircleShape current = pieces[board.index(placed)];
	int connected = 1;

	//searching horizontally
	//--horizontally-left
	checkPos.x = placed.x - 1;
	while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == current.getFillColor()) {
		if (++connected == numConnected) return WIN;
		checkPos.x--;
	}
	//--horizontally-right
	checkPos.x = placed.x + 1;
	while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == current.getFillColor()) {
		if (++connected == numConnected) return WIN;
		checkPos.x++;
	}

	checkPos = placed;
	connected = 1;

	//searching vertically
	//--vertically-down
	checkPos.y = placed.y + 1;
	while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == current.getFillColor()) {
		if (++connected == numConnected) return WIN;
		checkPos.y++;
	}
	//--vertically-up
	checkPos.y = placed.y - 1;
	while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == current.getFillColor()) {
		if (++connected == numConnected) return WIN;
		checkPos.y--;
	}

	connected = 1;

	//searching diagonally bottom left corner to top right corner
	//--diagonally-down
	checkPos.x = placed.x - 1;
	checkPos.y = placed.y + 1;
	while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == current.getFillColor()) {
		if (++connected == numConnected) return WIN;
		checkPos.x--;
		checkPos.y++;
	}
	//--diagonally-up
	checkPos.x = placed.x + 1;
	checkPos.y = placed.y - 1;
	while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == current.getFillColor()) {
		if (++connected == numConnected) return WIN;
		checkPos.x++;
		checkPos.y--;
	}

	connected = 1;

	//searching diagonally top left corner to bottom left corner
	//--diagonally-down
	checkPos.x = placed.x + 1;
	checkPos.y = placed.y + 1;
	while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == current.getFillColor()) {
		if (++connected == numConnected) return WIN;
		checkPos.x++;
		checkPos.y++;
	}
	//--diagonally-up
	checkPos.x = placed.x - 1;
	checkPos.y = placed.y - 1;
	while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == current.getFillColor()) {
		if (++connected == numConnected) return WIN;
		checkPos.x--;
		checkPos.y--;
	}

	return CONTINUE;
}

//Return the column the mouse is hovering over given the mouse's location in pixels
//If mouse is outside the boundaries of the screen, return -1
int Game::convertPixelsToColumn(sf::Vector2i& pos) {
	if (pos.x < 0 || pos.x > window.getSize().x || pos.y < 0 || pos.y > window.getSize().y) return -1;
	int numColumns = board.getWidth();
	float columnLength = window.getSize().x / numColumns;

	return (int) pos.x / columnLength;
}

void Game::drawMarker(int col) {
	marker.setFillColor(getCurrentColor());

	float xPos = 1.5 * marker.getRadius();
	xPos += col * 3 * marker.getRadius();
	marker.setPosition(xPos, 1.5 * marker.getRadius());
	window.draw(marker);
}

void Game::animateDrop(Position& placed) {

}

void Game::setEndText(sf::Text& text, sf::Color color) {
	sf::Font font;
	font.loadFromFile("fonts/Cousine-Regular.ttf");

	text.setFont(font);
	text.setFillColor(color);
	text.setCharacterSize(24);
	sf::FloatRect f = text.getLocalBounds();
	text.setOrigin(f.width / 2, 0);
}

void Game::drawEndText(Status gameStatus) {
	sf::Text winnerText;
	sf::Text instructions;
	sf::Color displayColor;

	if (gameStatus == TIE) {
		displayColor = sf::Color::White;
		winnerText.setString("It's a Tie!");
	}
	else {
		displayColor = getCurrentColor();
		if (displayColor == sf::Color::Yellow)   winnerText.setString("Yellow Wins!");
		else if (displayColor == sf::Color::Red) winnerText.setString("Red wins!");
		else                                     winnerText.setString("Game over");
	}

	//setEndText(winnerText, displayColor);
	//setEndText(instructions, displayColor);

	sf::Font font;
	font.loadFromFile("fonts/Cousine-Regular.ttf");

	winnerText.setFont(font);
	winnerText.setFillColor(displayColor);
	winnerText.setCharacterSize(24);
	sf::FloatRect f = winnerText.getLocalBounds();
	winnerText.setOrigin(f.width / 2, 0);

	winnerText.setPosition(window.getSize().x / 2, 0);
	window.draw(winnerText);

	instructions.setFont(font);
	instructions.setString("Press Enter to Play Again or Escape to Quit");
	instructions.setFillColor(displayColor);
	instructions.setCharacterSize(24);
	sf::FloatRect f2 = instructions.getLocalBounds();
	instructions.setOrigin(f2.width / 2, 0);
	instructions.setPosition(window.getSize().x / 2, winnerText.getLocalBounds().height + 10);
	window.draw(instructions);
}
