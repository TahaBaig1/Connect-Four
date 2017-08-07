#ifndef GAME_H
#define GAME_H

#include "Board.h"

enum Status {
	CONTINUE,TIE,WIN
};

//contains game loop and all game components/logic
class Game {
	private:
		int turn; //current turn number
		int numConnected; //number of pieces required to be connected in a line in order to win
		sf::RenderWindow window;
		Board board;
		sf::CircleShape marker;

		sf::Color getCurrentColor();
		Status checkWon(Position placed);
		int convertPixelsToColumn(sf::Vector2i pos);
		void drawMarker(int col);
		void animateDrop(Position p);
	public:
		Game(int numConnected_ = 4, int turn_ = 1);
		void run(); //game loop
};

#endif