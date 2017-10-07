#ifndef BOARD_H
#define BOARD_H

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "Position.h"

//Connect 4 Game Board -- Includes graphics information of board and pieces alongside storing pieces data
class Board {
	public:
		Board(sf::RenderWindow& window_, int width_ = 7, int height_ = 6);
		~Board();
		
		int getWidth() const;
		int getHeight() const;
		sf::CircleShape* getPieces() const;
		sf::Color getColor1() const;
		sf::Color getColor2() const;
		sf::Color getBackgroundColor() const;

		void drawBoard();

		Position addPiece(int col, const sf::Color& color);
	    int index(Position& p) const;
		bool validPosition(Position& p) const;
		void clear();
	private:
		//graphics for game board and game pieces
		sf::RenderWindow& window;
		sf::RectangleShape board;
		sf::CircleShape* pieces; //1D array holding all the pieces of the board. Will be treated as if it is a 2D array to keep track of piece locations
		sf::Color pieceColor1;
		sf::Color pieceColor2;
		sf::Color backgroundColor;

		//width represents the number of discs allowed horizontally on the board while height represents number of discs allowed vertically
		int width;
		int height;
		
		//numInColumns indicates the current number of pieces occuyping each column on the board
		int* numInColumns;

};
#endif