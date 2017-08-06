#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "Position.h"

//Connect 4 Game Board
class Board {
	public:
		Board(int width_ = 7, int height_ = 6);
		~Board();
		
		int getWidth() const;
		int getHeight() const;
		sf::CircleShape* getPieces() const;
		sf::RenderWindow& getWindow();

		void drawBoard();
		Position addPiece(int col, sf::Color color);
	    int index(Position p);
		bool validPosition(Position p);
	private:
		//width represents the number of dics allowed horizontally on the board while height represents number of discs allowed vertically
		int width;
		int height;
		
		//numInColumns indicates the current number of pieces occuyping each column on the board
		int* numInColumns;

		//graphics for game board and game pieces
		sf::RenderWindow window;
		sf::RectangleShape board;
		sf::CircleShape* pieces; //1D array holding all the pieces of the board. Will be treated as if it is a 2D array to keep track of piece locations
};
#endif