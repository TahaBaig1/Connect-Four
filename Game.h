#ifndef GAME_H
#define GAME_H

#include <SFML/Audio.hpp>
#include "Board.h"

enum Status {
	CONTINUE,TIE,WIN
};

//contains game loop and all game components/logic
class Game {
	public:
		Game(sf::RenderWindow& window, int numConnected_ = 4, int turn_ = 1);
		void run(); //game loop
	private:
		int turn; //current turn number
		int numConnected; //number of pieces required to be connected in a line in order to win
		sf::RenderWindow& window;
		Board board;
		sf::CircleShape marker;

		//for sound that plays when piece is dropped on board
		sf::SoundBuffer buffer;
		sf::Sound dropSound;

		sf::Color getCurrentColor() const;
		Status isGameOver(Position& placed) const;
		int convertMousePosToColumn(sf::Vector2i& pos) const;
		void drawMarker(int col);
		void animateDrop(Position& placed);
		bool endLoop(Status gameStatus);
		void setEndText(sf::Text& text, sf::Font& font, sf::Color& color);
		void drawEndText(Status gameStatus);
		void reset();
};

#endif