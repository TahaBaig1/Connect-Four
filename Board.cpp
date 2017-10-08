#include "Board.h"



Board::Board(sf::RenderWindow& window_, int width_, int height_) : window(window_), width(width_), height(height_) {
	width = 6;
	height = 5;
	//Setting up piece graphics to be drawn on window later
	//Pieces are circular disks of a fixed radius window (1/3) * [pixel width/(number of columns (width) )] 
	//By default they are set to the background color to represent no piece currently occupying that position on the board
	pieceColor1 = sf::Color::Red;
	pieceColor2 = sf::Color::Yellow;
	backgroundColor = sf::Color::Color(170, 196, 239);
	sf::Color borderColor(2, 37, 94);

	float pieceRadius = window.getSize().x / width / 3;
	pieces = new sf::CircleShape[width*height];

	for (int row = 0; row < width; row++) {
		for (int col = 0; col < height; col++) {
			sf::CircleShape piece(pieceRadius);
			piece.setFillColor(backgroundColor);
			piece.setOrigin(pieceRadius, pieceRadius);
			piece.setOutlineThickness(5);
			piece.setOutlineColor(borderColor);
			pieces[index(Position(row, col))] = piece;
		} 
	}

	//Setting up game board graphics to be drawn on window later
	//Board will occupy full height/width of the window with the exception of slight area above the board used for displaying other information
	board.setSize(sf::Vector2f(window.getSize().x, window.getSize().y - 3 * pieceRadius));
	board.setFillColor(sf::Color(71,133,228));
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

int * Board::getNumInColumns() const
{
	return numInColumns;
}

sf::Color Board::getColor1() const{
	return pieceColor1;
}

sf::Color Board::getColor2() const{
	return pieceColor2;
}

sf::Color Board::getBackgroundColor() const {
	return backgroundColor;
}


void Board::drawBoard() {
	window.draw(board);

	//Drawing discs on board based on pieces array
	//Center of discs on the board are 1.5 times their radius away from the edges of the board and 3 times their radius away from each other
	float pieceRadius = pieces[0].getRadius();
	float currentX = board.getPosition().x + 1.5*pieceRadius;
	float currentY = board.getPosition().y + 1.5*pieceRadius;
	for (int i = 0; i < height; i++) {
		pieces[index(Position(0, i))].setPosition(currentX, currentY);
		window.draw(pieces[index(Position(0, i))]);
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
//If specified column of board is already filled or is invalid, returns Position(-1,-1), does not increment numInColumns
//Note: Positions x and y coordinates are the same as a 2D array would be [e.g. top left corner is at (0,0), below is (0,1) and right is (1,0)]
Position Board::addPiece(int col, const sf::Color& color) {
	if (col >= width) return Position(-1, -1);

	int numInCol = numInColumns[col];
	if (numInCol == height) return Position(-1, -1);
	Position p(col, height - numInCol - 1);
	pieces[index(p)].setFillColor(color);
	numInColumns[col] += 1;
	return p;
}

//removes piece at top of specified column if piece is there and decrements numInColumns
//If specified column of board is invalid, returns Position(-1,-1) does not decrement numInColumns
void Board::removePiece(int col) {
	Position p(col, height - numInColumns[col]);
	if (!validPosition(p)) return;

	if (pieces[index(p)].getFillColor() != backgroundColor) {
		pieces[index(p)].setFillColor(backgroundColor);
		numInColumns[col] -= 1;
	}
}

//Converts 2D dimenions into 1D dimensions
int Board::index(Position& p) const{
	return p.y * width + p.x;
}

int Board::index(int x, int y) const {
	return y * width + x;
}

//Checks whether 2D dimensions are valid on board
bool Board::validPosition(Position& p) const{
	return (p.x >= 0 && p.x < width) && (p.y >= 0 && p.y < height);
}

void Board::clear() {
	//resetting all discs on board
	for (int i = 0; i < width*height; i++) {
		pieces[i].setFillColor(backgroundColor);
	}

	//setting number in all columns to zero
	for (int i = 0; i < width; i++) {
		numInColumns[i] = 0;
	}
}

