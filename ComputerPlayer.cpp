#include "ComputerPlayer.h"
#include <cstdlib>
#include <iostream>
#include <unordered_set>

ComputerPlayer::ComputerPlayer() {}

ComputerPlayer::ComputerPlayer(sf::Color aiColor, sf::Color opposingColor_, int numConnected_, int difficulty_) : color(aiColor), 
																													  opposingColor(opposingColor_), 
																													  numConnected(numConnected_),
																													  difficulty(difficulty_) {}

void ComputerPlayer::setColors(sf::Color aiColor, sf::Color opposingColor_) {
	this->color = aiColor;
	this->opposingColor = opposingColor_;
}

void ComputerPlayer::setNumConnected(int num) {
	this->numConnected = num;
}

void ComputerPlayer::setDifficulty(int difficulty) {
	this->difficulty = difficulty;
}

Position ComputerPlayer::makeMove(Board& board, int turn) const {
	int col = minimax(board);
	Position placed = board.addPiece(col, this->color);
	return placed;
}

//returns optimal move (column number)
int ComputerPlayer::minimax(Board& board) const{
	std::vector<int> moves;
	getPossibleMoves(board, moves); //gets possible moves AI can make

	int alpha = -1000000; //arbritary large negative number, best choice so far for AI (so maximal choice) (initially set to worst value)
	int beta = 1000000;   //arbritary large posistive number, best choice so far for human (so minimal choice) (initially set to worst value)

	//Initially AI's turn: we want the maximum score possible
	int bestMoveScore = -1000000; //arbritary large number (best move score so far, initially at worst value)

	std::vector<int> bestEquivalentMoves;

	for (int i = 0; i < moves.size(); i++) {
		Position placed = board.addPiece(moves[i], this->color);

		//AI has added a piece, it is now either terminal state or human's turn (human will minimize)
		//the difficulty member of ComputerPlayer will become the depth of search of the maximizing/minimizing functions
		int moveScore = minimize(board, placed, alpha, beta, this->difficulty); 
		std::cout << moves[i] << ":" << moveScore << " ";
		if (moveScore > bestMoveScore) {
			bestMoveScore = moveScore;
			bestEquivalentMoves.clear();
			bestEquivalentMoves.push_back(moves[i]);
			alpha = moveScore;
		}
		else if (moveScore == bestMoveScore) {
			bestEquivalentMoves.push_back(moves[i]);
		}
		board.removePiece(moves[i]);
	}

	std::cout << std::endl;
	return bestEquivalentMoves[rand() % bestEquivalentMoves.size()];
}

int ComputerPlayer::minimize(Board& board, Position placed, int alpha, int beta, int depth) const{
	Status gameStatus = isGameOver(board, placed);
	if (gameStatus == WIN) return 1000 + depth;
	if (gameStatus == TIE) return 0 + depth;
	if (depth <= 0) return eval(board, this->color);

	std::vector<int> moves;
	getPossibleMoves(board, moves); //gets possible moves Human can make

	//Human wants to minimize the score 
	int bestMoveScore = 1000000; //arbritary large number (best move score so far, initially at worst value)

	for (int i = 0; i < moves.size(); i++) {
		Position placed = board.addPiece(moves[i], this->opposingColor);
		int moveScore = maximize(board, placed, alpha, beta, depth-1); //Human has added a piece, it now either terminal sate or AI's turn (AI will maximize)
		if (moveScore < beta) { //this is the minimizing step, so beta's value changes as new best minimums are found
			beta = moveScore;
		}
		if (moveScore < bestMoveScore) {
			bestMoveScore = moveScore;
		}
		board.removePiece(moves[i]);

		//if at any point we find that beta <= alpha, this means that it is assurred that the minimum value for this move is less than./equal the maximum value
		//then we can continue and break out of the possible next moves as it is known that this move is a worse/equivalent choice 
		if (beta <= alpha) break; 
	}

	return bestMoveScore;
}

int ComputerPlayer::maximize(Board & board, Position placed, int alpha, int beta, int depth) const {
	Status gameStatus = isGameOver(board, placed);
	if (gameStatus == WIN) return -1000 - depth;
	if (gameStatus == TIE) return 0 - depth;
	if (depth <= 0) return -1 * eval(board, this->opposingColor);

	std::vector<int> moves;
	getPossibleMoves(board, moves); //gets possible moves AI can make

	//AI wants to maximize score
	int bestMoveScore = -1000000; //arbritary large negative number (best move score so far, intially set to worst value)

	for (int i = 0; i < moves.size(); i++) {
		Position placed = board.addPiece(moves[i], this->color);
		int moveScore = minimize(board, placed, alpha, beta, depth-1); //AI has added a piece, it is now either terminal state or human's turn (human will minimize)
		if (moveScore > alpha) {
			alpha = moveScore;
		}
		if (moveScore > bestMoveScore) {
			bestMoveScore = moveScore;
		}
		board.removePiece(moves[i]);

		//if at any point we find that beta <= alpha, this means that it is assurred that the minimum value for this move is less than/equal to the maximum value 
		//then we can continue and break out of the possible next moves as it is known that this move is a worse/equivalent choice
		if (beta <= alpha) break;
	}

	return bestMoveScore;
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

//heuristic evaluvation function for non-terminal state boards
//goes through entire board and counts number of (useful) connections in order to assign a heuristic value of current board
//a useful connection is one that can be expanded (if a 3 in a row is surrounded by opponent pieces or out of bounds, it is not useful)
int ComputerPlayer::eval(Board& board, sf::Color color) const {

	int val = 0;
	sf::CircleShape* pieces = board.getPieces();
	std::unordered_set<int> visited;

	for (int row = 0; row < board.getHeight(); row++) {
		for (int col = 0; col < board.getWidth(); col++) {
			Position curr(col, row);
			if (visited.count(board.index(curr)) > 0) continue; //avoiding double counting number of connections

			const sf::CircleShape& current = pieces[board.index(curr)];
			if (current.getFillColor() == board.getBackgroundColor()) continue; //empty slot, move on
			if (current.getFillColor() != color) continue; //opponent spot, move on 

			int connected = 1;

			//horizontal connections:

			//horizontal left
			Position checkPos = curr;
			checkPos.x = curr.x - 1;
			while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == color) {
				connected++;
				visited.insert(board.index(checkPos));
				checkPos.x--;
			}
			Position before = checkPos;

			//horizontal right
			checkPos.x = curr.x + 1;
			while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == color) {
				connected++;
				visited.insert(board.index(checkPos));
				checkPos.x++;
			}
			Position after = checkPos;

			val += getConnectedVal(before, after, connected, board);

			//vertical connections
			connected = 1;
			checkPos = curr;

			//vertical down
			checkPos.y = curr.y + 1;
			while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == color) {
				connected++;
				visited.insert(board.index(checkPos));
				checkPos.y++;
			}
			before = checkPos;

			//vertical up
			checkPos.y = curr.y - 1;
			while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == color) {
				connected++;
				visited.insert(board.index(checkPos));
				checkPos.y--;
			}
			after = checkPos;

			val += getConnectedVal(before, after, connected, board);

			//bottom left to top right diagonal connections
			connected = 1;
			checkPos = curr;

			//diagonally down
			checkPos.x = curr.x - 1;
			checkPos.y = curr.y + 1;
			while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == color) {
				connected++;
				visited.insert(board.index(checkPos));
				checkPos.x--;
				checkPos.y++;
			}
			before = checkPos;

			//diagonally up
			checkPos.x = curr.x + 1;
			checkPos.y = curr.y - 1;
			while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == color) {
				connected++;
				visited.insert(board.index(checkPos));
				checkPos.x++;
				checkPos.y--;
			}
			after = checkPos;

			val += getConnectedVal(before, after, connected, board);

			//top left to bottom right diagonal connections
			connected = 1;
			checkPos = curr;

			//diagonally down
			checkPos.x = curr.x + 1;
			checkPos.y = curr.y + 1;
			while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == color) {
				connected++;
				visited.insert(board.index(checkPos));
				checkPos.x++;
				checkPos.y++;
			}
			before = checkPos;

			//diagonally up
			checkPos.x = curr.x - 1;
			checkPos.y = curr.y - 1;
			while (board.validPosition(checkPos) && pieces[board.index(checkPos)].getFillColor() == color) {
				connected++;
				visited.insert(board.index(checkPos));
				checkPos.x--;
				checkPos.y--;
			}
			after = checkPos;

			val += getConnectedVal(before, after, connected, board);
		}
	}

	return val;
}

int ComputerPlayer::getConnectedVal(Position before, Position after, int connected, Board& board) const{
	//a connection is not useful if both before and after the connection the board is out of bounds or occupied by the opposing color
	if ( (!board.validPosition(before) || board.getPieces()[board.index(before)].getFillColor() != board.getBackgroundColor()) &&
		 (!board.validPosition(after) || board.getPieces()[board.index(after)].getFillColor() != board.getBackgroundColor() ) ) {
		return 0;
	}

	return connected * connected;
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



