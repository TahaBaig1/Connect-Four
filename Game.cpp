#include "Game.h"
#include <SFML/Audio.hpp>
#include "ComputerPlayer.h"

Game::Game(sf::RenderWindow& window_, int numConnected_, int turn_) : numConnected(numConnected_), 
																	  turn(turn_), 
																	  window(window_),
																	  board(window_)
{
	//setting up graphics for game marker
	float markerRadius = board.getPieces()[0].getRadius();
	marker.setRadius(markerRadius);
	marker.setOrigin(markerRadius, markerRadius);

	//setting up sound effects
	buffer.loadFromFile("sounds/drop.wav");
	dropSound.setBuffer(buffer);
}

//game loop
void Game::run() {
	bool playGame = true;
	//as long as player wants to continue playing game, reset game and keep running
	while (playGame) {
		Mode gameMode = displayTitleScreen();
		Status gameStatus = CONTINUE;
		ComputerPlayer ai;
		ai.setNumConnected(numConnected);
		ai.setDifficulty(4);
		
		//setup AI if not player vs player
		//if AI moves first gamemode, then let AI take first turn
		if (gameMode == AI_FIRST) {
			ai.setColors(board.getColor1(), board.getColor2());
			Position placed = ai.makeMove(board, turn);
			drawMarker(placed.x);
			animateDrop(placed);
			dropSound.play();
			turn++;
		} else if (gameMode == AI_SECOND) {
			ai.setColors(board.getColor2(), board.getColor1());
		}

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
					drawMarker(placed.x);
					animateDrop(placed);
					dropSound.play();

					window.clear(board.getBackgroundColor());
					board.drawBoard();
					window.display();

					//determine whether game has ended; if not, increment the turn number
					gameStatus = isGameOver(placed);
					if (gameStatus != CONTINUE) break;
				    turn++;

					//run AI turn if gamemode is not player vs player
					if (gameMode == AI_FIRST || gameMode == AI_SECOND) {
						Position placed = ai.makeMove(board, turn);
						drawMarker(placed.x);
						animateDrop(placed);
						dropSound.play();
						gameStatus = isGameOver(placed);
						if (gameStatus != CONTINUE) break;
						turn++;
					}
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

//generates title screen loop and determines if player wishes to play against another player or an AI 
Mode Game::displayTitleScreen() const{
	//generate title display text
	window.clear(board.getBackgroundColor());

	sf::Font font;
	font.loadFromFile("fonts/Cousine-Regular.ttf");

	sf::Text title, gameMode1, gameMode2, gameMode3;
	title.setString("CONNECT FOUR");
	gameMode1.setString("1. Player vs Player Game");
	gameMode2.setString("2. Player vs Computer (Computer moves first)");
	gameMode3.setString("3. Player vs Computer (Computer moves second)");

	sf::Color titleTextColor(sf::Color::Red);
	sf::Color gameModesColor(sf::Color::Black);

	setText(title, font, titleTextColor, 48);
	title.setPosition(window.getSize().x / 2, 0);
	window.draw(title);
	setText(gameMode1, font, gameModesColor, 24);
	gameMode1.setPosition(window.getSize().x/2, window.getSize().y / 3);
	window.draw(gameMode1);
	setText(gameMode2, font, gameModesColor, 24);
	gameMode2.setPosition(window.getSize().x/2, window.getSize().y / 3  + 2*gameMode1.getLocalBounds().height);
	window.draw(gameMode2);
	setText(gameMode3, font, gameModesColor, 24);
	gameMode3.setPosition(window.getSize().x/2, window.getSize().y / 3 + 4*gameMode1.getLocalBounds().height);
	window.draw(gameMode3);

	window.display();

	//loop until player decides on a game mode
	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) window.close();
			if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Num1 || event.key.code == sf::Keyboard::Numpad1)) {
				window.clear();
				return PLAYER_V_PLAYER;
			}
			if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Num2 || event.key.code == sf::Keyboard::Numpad2)) {
				window.clear();
				return AI_FIRST;
			}
			if (event.type == sf::Event::KeyPressed && (event.key.code == sf::Keyboard::Num3 || event.key.code == sf::Keyboard::Numpad3)) {
				window.clear();
				return AI_SECOND;
			}

		}
	}
	
	//should never get here
	window.clear();
	return PLAYER_V_PLAYER;
}

sf::Color Game::getCurrentColor() const{
	if (turn % 2 == 0) return board.getColor2();
	else               return board.getColor1();
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

void Game::setText(sf::Text& text, sf::Font& font, sf::Color& color, int charSize) const{
	text.setFont(font);
	text.setFillColor(color);
	text.setCharacterSize(charSize);
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

	setText(winnerText, font, displayColor, 24);
	winnerText.setPosition(window.getSize().x / 2, 0);
	window.draw(winnerText);

	instructions.setString("Press Enter to Play Again or Escape to Quit");
	setText(instructions, font, displayColor, 24);
	instructions.setPosition(window.getSize().x / 2, winnerText.getLocalBounds().height + window.getSize().y / 100);
	window.draw(instructions);
}
