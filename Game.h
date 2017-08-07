#ifndef GAME_H
#define GAME_H

#include "Board.h"

//contains game loop and all game components/logic
class Game {
	private:
		int turn;
		int numConnected;
		sf::RenderWindow window;
		Board board;
		sf::CircleShape marker;
	public:
		Game(int numConnected_ = 4, int turn_ = 1);
		void run(); //game loop
		int checkWon(Position last);
		int convertPixelsToColumn(sf::Vector2i pos);
		void drawMarker(int col);
		void animateDrop(Position p);
};

#endif