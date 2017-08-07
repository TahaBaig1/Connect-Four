#include "Board.h"
#include <iostream>

Board::Board(sf::RenderWindow& window_, int width_, int height_) : window(window_), width(width_), height(height_) {

	//Setting up piece graphics to be drawn on window later
	//Pieces are circular disks of a fixed radius 1/(width * window pixel width) 
	//By default they are set to the color black to represent no piece currently occupying that position on the board
	float pieceRadius = window.getSize().x / width / 3;
	pieces = new sf::CircleShape[width*height];

	for (int row = 0; row < width; row++) {
		for (int col = 0; col < height; col++) {
			sf::CircleShape piece(pieceRadius);
			piece.setFillColor(sf::Color::Black);
			piece.setOrigin(pieceRadius, pieceRadius);
			pieces[index(Position(row, col))] = piece;
		} 
	}

	//Setting up game board graphics to be drawn on window later
	//Board will occupy full height/width of the window with the exception of slight area above the board used for displaying other information
	board.setSize(sf::Vector2f(window.getSize().x, window.getSize().y - 3 * pieceRadius));
	board.setFillColor(sf::Color::Yellow);
	board.setPosition(0, 3 * pieceRadius);

	numInColumns = new int[width];
	for(int i = 0; i < width; i++){
		numInColumns[i] = 0;
	}
}

Board::~Board() {
	delete [](pieces);
	delete [](numInColumns);
}

int Board::getWidth() const {
	return width;
}

int Board::getHeight() const {
	return height;
}

sf::CircleShape * Board::getPieces() const {
	return pieces;
}

void Board::drawBoard() {
	window.draw(board);

	//Drawing discs on board based on pieces array
	//Center of discs on the board are 1.5 times their radius away from the edges of the board and 3 times their radius away from each other
	float pieceRadius = pieces[0].getRadius();
	float currentX = board.getPosition().x + 1.5*pieceRadius;
	float currentY = board.getPosition().y + 1.5*pieceRadius;
	for (int i = 0; i < height; i++) {
		pieces[index(Position(0,i))].setPosition(currentX, currentY);
		window.draw(pieces[index(Position(0,i))]);
		for (int j = 1; j < width; j++) {
			currentX += 3 * pieceRadius;
			pieces[index(Position(j, i))].setPosition(currentX, currentY);
			window.draw(pieces[index(Position(j, i))]);
		}
		currentY += 3 * pieceRadius;
		currentX = board.getPosition().x + 1.5*pieceRadius;
	}
}


//Adds piece of specified color to specified column of board, increments numInColumns, and returns Position filled
//If specified column of board is already filled, returns Position(-1,-1), does not increment numInColumns
Position Board::addPiece(int col, sf::Color color) {
	int numInCol = numInColumns[col];
	if (numInCol == height) return Position(-1, -1);
	Position p(col, height - numInCol - 1);
	pieces[index(p)].setFillColor(color);
	numInColumns[col] += 1;
	return p;
}

int Board::index(Position p) {
	return p.y * width + p.x;
}

bool Board::validPosition(Position p) {
	return (p.x >= 0 && p.x < width) && (p.y >= 0 && p.y < height);
}

