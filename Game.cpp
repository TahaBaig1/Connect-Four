#include "Game.h"
#include <SFML/Audio.hpp>

Game::Game(sf::RenderWindow& window_, int numConnected_, int turn_) : numConnected(numConnected_), 
																	  turn(turn_), 
																	  window(window_),
																	  board(window_)
{
	//setting up graphics for game marker
	float markerRadius = window.getSize().x / board.getWidth() / 3;
	marker.setRadius(markerRadius);
	marker.setOrigin(markerRadius, markerRadius);

	buffer.loadFromFile("sounds/drop.wav");
	dropSound.setBuffer(buffer);
}

//game loop
void Game::run() {
	bool playGame = true;

	//as long as player wants to continue playing game, reset game and keep running
	while (playGame) {
		Status gameStatus = CONTINUE;

		//Playing game
		while (window.isOpen()) {
			sf::Event event;
			while (window.pollEvent(event)) {
				if (event.type == sf::Event::Closed) window.close();
				if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();
				if (event.type == sf::Event::MouseButtonPressed) {
					//get column and position on boad where piece will be dropped
					int col = convertMousePosToColumn(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					if (col == -1)  break;
					Position placed = board.addPiece(col, getCurrentColor());
					if (placed.x == -1 || placed.y == -1) break;

					//animate dropping and play sound effect at end
					animateDrop(placed);
					dropSound.play();

					//determine whether game has ended; if not, increment the turn number
					gameStatus = isGameOver(placed);
					if (gameStatus != CONTINUE) break;
				    turn++;
				}
			}
			if (gameStatus != CONTINUE) break;
			
			//draw board and board marker hovering over column user is currently mousing over
			window.clear(board.getBackgroundColor());
			board.drawBoard();
			int currentCol = convertMousePosToColumn(sf::Mouse::getPosition(window));
			if (currentCol != -1) drawMarker(currentCol);
			window.display();
		}

		playGame = endLoop(gameStatus); //Displaying end game text when game is over and returning whether player wants to play again or exit
		if (playGame) reset();	//If player does want to play again, reset game information
	}
}

sf::Color Game::getCurrentColor() const{
	if (turn % 2 == 0) return board.getColor1();
	else               return board.getColor2();
}

//checks game board and returns current game status (CONTINUE, TIE, WIN)
//works via passing in last position where a piece was dropped and searching horizontally, vertically, and diagonally from piece to find connections
Status Game::isGameOver(Position& placed) const{
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
int Game::convertMousePosToColumn(sf::Vector2i& pos) const{
	if (pos.x < 0 || pos.x > window.getSize().x || pos.y < 0 || pos.y > window.getSize().y) return -1;
	int numColumns = board.getWidth();
	float columnLength = window.getSize().x / numColumns;

	return (int) pos.x / columnLength;
}

//displays marker indicating which color is currently making a move above column user mouse is currently hovering over
void Game::drawMarker(int col) {
	marker.setFillColor(getCurrentColor());

	float xPos = 1.5 * marker.getRadius();
	xPos += col * 3 * marker.getRadius();
	marker.setPosition(xPos, 1.5 * marker.getRadius());
	window.draw(marker);
}

void Game::animateDrop(Position& placed) {
	const float velocity = 3; //pixels per millisecond
	sf::Clock clock;
	
	float yCurrent = marker.getPosition().y;
	float yFinal = 1.5 * marker.getRadius() + (placed.y + 1) * 3 * marker.getRadius();
	float xPos = 1.5 * marker.getRadius() + (placed.x) * 3 * marker.getRadius();
	sf::CircleShape tempBlocker(marker.getRadius());
	tempBlocker.setFillColor(board.getBackgroundColor());
	tempBlocker.setOrigin(tempBlocker.getRadius(), tempBlocker.getRadius());
	tempBlocker.setPosition(xPos, yFinal);

	while (yCurrent < yFinal) {
		marker.setPosition(xPos, yCurrent);
		window.clear(board.getBackgroundColor());
		board.drawBoard();
		window.draw(tempBlocker);
		window.draw(marker);
		window.display();

		yCurrent += velocity * clock.getElapsedTime().asMilliseconds();
		clock.restart();
	}

}

bool Game::endLoop(Status gameStatus) {
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type = sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Escape) window.close();
				if (event.key.code == sf::Keyboard::Return)  return true;
			}
		}

		window.clear(board.getBackgroundColor());
		board.drawBoard();
		drawEndText(gameStatus);
		window.display();
	}

	return false;
}

void Game::reset() {
	turn = 1;
	board.clear();
}

void Game::setEndText(sf::Text& text, sf::Font& font, sf::Color& color) {
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
		if (displayColor == board.getColor1())      winnerText.setString("Red Wins!");
		else if (displayColor == board.getColor2()) winnerText.setString("Yellow wins!");
		else                                        winnerText.setString("Game over");
	}

	sf::Font font;
	font.loadFromFile("fonts/Cousine-Regular.ttf");

	setEndText(winnerText, font, displayColor);
	winnerText.setPosition(window.getSize().x / 2, 0);
	window.draw(winnerText);

	instructions.setString("Press Enter to Play Again or Escape to Quit");
	setEndText(instructions, font, displayColor);
	instructions.setPosition(window.getSize().x / 2, winnerText.getLocalBounds().height + window.getSize().y / 100);
	window.draw(instructions);
}
