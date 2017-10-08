#include "ComputerPlayer.h"
#include <cstdlib>
#include <iostream>

ComputerPlayer::ComputerPlayer() {}

ComputerPlayer::ComputerPlayer(sf::Color aiColor, sf::Color opposingColor_, int numConnected_) : color(aiColor), opposingColor(opposingColor_), numConnected(numConnected_){}

void ComputerPlayer::setColors(sf::Color aiColor, sf::Color opposingColor_) {
	this->color = aiColor;
	this->opposingColor = opposingColor_;
}

void ComputerPlayer::setNumConnected(int num) {
	numConnected = num;
}


Position ComputerPlayer::makeMove(Board& board, int turn) const {
	if (turn == 1) {
		return board.addPiece(0, this->color);
	}
	int col = minimax(board);
	Position placed = board.addPiece(col, this->color);
	return placed;
}

//returns optimal move (column number)
int ComputerPlayer::minimax(Board& board) const{
	std::vector<int> moves;
	getPossibleMoves(board, moves); //gets possible moves AI can make

	int bestMoveIndex = 0; //starting position

	int alpha = -1000000; //arbritary large negative number, best choice so far for AI (so maximal choice)
	int beta = 1000000;   //arbritary large posistive number, best choice so far for human (so minimal choice)

	//Initially AI's turn: we want the maximum score possible
	for (int i = 0; i < moves.size(); i++) {
		Position placed = board.addPiece(moves[i], this->color);
		int moveScore = minimize(board, placed, alpha, beta); //AI has added a piece, it is now either terminal state or human's turn (human will minimize)
		std::cout << moveScore << " ";
		if (moveScore > alpha) {
			alpha = moveScore;
			bestMoveIndex = i;
		}
		board.removePiece(moves[i]);
	}

	std::cout << std::endl;
	return moves[bestMoveIndex];
}

int ComputerPlayer::minimize(Board& board, Position placed, int alpha, int beta) const{
	Status gameStatus = isGameOver(board, placed);
	if (gameStatus == WIN) return 1;
	if (gameStatus == TIE) return 0;

	std::vector<int> moves;
	getPossibleMoves(board, moves); //gets possible moves Human can make

	//Human wants to minimize the score 

	for (int i = 0; i < moves.size(); i++) {
		Position placed = board.addPiece(moves[i], this->opposingColor);
		int moveScore = maximize(board, placed, alpha, beta); //Human has added a piece, it now either terminal sate or AI's turn (AI will maximize)
		if (moveScore < beta) {
			beta = moveScore;
		}
		board.removePiece(moves[i]);
		if (beta <= alpha) break;
	}

	return beta;
}

int ComputerPlayer::maximize(Board & board, Position placed, int alpha, int beta) const {
	Status gameStatus = isGameOver(board, placed);
	if (gameStatus == WIN) return -1;
	if (gameStatus == TIE) return 0;

	std::vector<int> moves;
	getPossibleMoves(board, moves); //gets possible moves AI can make

	//AI wants to maximize score
	
	for (int i = 0; i < moves.size(); i++) {
		Position placed = board.addPiece(moves[i], this->color);
		int moveScore = minimize(board, placed, alpha, beta); //AI has added a piece, it is now either terminal state or human's turn (human will minimize)
		if (moveScore > alpha) {
			alpha = moveScore;
		}
		board.removePiece(moves[i]);
		if (beta <= alpha) break;
	}

	return alpha;
}

//returns vector filled with all possible moves on board (each 'move' is just  a column number)
void ComputerPlayer::getPossibleMoves(Board& board, std::vector<int>& moves) const{
	int* numInCols = board.getNumInColumns();
	for (int col = 0; col < board.getWidth(); col++) {
		if (numInCols[col] < board.getHeight()) {
			moves.push_back(col);
		}
	}
}

//determines if current board is a Win state, Tie State, or neither
Status ComputerPlayer::isGameOver(Board& board, Position& placed) const {
	Position checkPos = placed;
	sf::CircleShape* pieces = board.getPieces();
	sf::CircleShape current = pieces[board.index(placed)];
	int connected = 1;

	//check for tie state
	int* numInCols = board.getNumInColumns();
	bool isTie = true;
	for (int col = 0; col < board.getWidth(); col++) {
		if (numInCols[col] < board.getHeight()) {
			isTie = false;
			break;
		}
	}
	if (isTie) return TIE;

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


//makes random move on board
Position ComputerPlayer::makeRandomMove(Board& board) const {

	Position placed(-1, -1);
	while (placed.x == -1 || placed.y == -1) {
		int col = rand() % board.getWidth();
		placed = board.addPiece(col, this->color);
	}

	return placed;
}


//convert Board of CircleShapes/pieces from Board object to a simpler, more convenient array of integers (1 for AI piece, -1 for human piece, 0 for no piece)
//int* ComputerPlayer::convertBoard(Board& board) const {
//	int* newBoard = new int[board.getHeight() * board.getWidth()];
//
//	for (int row = 0; row < board.getHeight(); row++) {
//		for (int col = 0; col < board.getWidth(); col++) {
//			int index = board.index(row, col);
//			sf::CircleShape& temp = board.getPieces()[index];
//
//			if (temp.getFillColor() == this->color)						newBoard[index] = 1;
//			else if (temp.getFillColor() == board.getBackgroundColor()) newBoard[index] = 0;
//			else                                                        newBoard[index] = -1;
//		}
//	}
//
//	return newBoard;
//}
