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
		int convertPixelsToColumn(float x);
		void drawMarker();
		void animateDrop(Position p);
};