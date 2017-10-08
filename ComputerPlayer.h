#ifndef COMPUTER_PLAYER_H
#define COMPUTER_PLAYER_H

#include "Board.h"
#include "Game.h"
#include <vector>

enum Player {
	AI, HUMAN
};

//struct GameState {
//	Board& board;
//	Position lastMove;
//	GameState(Board&b, Position p) : board(b), lastMove(p) {}
//};

class ComputerPlayer {
	public:
		ComputerPlayer();
		ComputerPlayer(sf::Color aiColor, sf::Color opposingColor, int numConnected_);

		void setColors(sf::Color aiColor, sf::Color opposingColor_);
		void setNumConnected(int num);
		Position makeMove(Board& board, int turn) const;
		Position makeRandomMove(Board & board) const;
	private:
		sf::Color color;
		sf::Color opposingColor;
		int numConnected;

		int minimax(Board& board) const;
		int minimize(Board & board, Position placed, int alpha, int beta) const;
		int maximize(Board & board, Position placed, int alpha, int beta) const;
		void getPossibleMoves(Board& board, std::vector<int>& moves) const;
		Status isGameOver(Board& board, Position& placed) const;
		//int* convertBoard(Board& board) const;
};

#endif