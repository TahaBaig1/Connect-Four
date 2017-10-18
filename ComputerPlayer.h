#ifndef COMPUTER_PLAYER_H
#define COMPUTER_PLAYER_H

#include "Board.h"
#include "Game.h"
#include <vector>

class ComputerPlayer {
	public:
		ComputerPlayer();
		ComputerPlayer(sf::Color aiColor, sf::Color opposingColor, int numConnected_, int difficulty_);

		void setColors(sf::Color aiColor, sf::Color opposingColor_);
		void setNumConnected(int num);
		void setDifficulty(int difficulty);

		Position makeMove(Board& board, int turn) const;
		Position makeRandomMove(Board & board) const;
	private:
		sf::Color color;
		sf::Color opposingColor;
		int numConnected;
		int difficulty; //how hard the AI will be: utilized in minimax algorithm as 'depth' of search

		int minimax(Board& board) const;
		int minimize(Board & board, Position placed, int alpha, int beta, int depth) const;
		int maximize(Board & board, Position placed, int alpha, int beta, int depth) const;
		void getPossibleMoves(Board& board, std::vector<int>& moves) const;
		Status isGameOver(Board& board, Position& placed) const;
		int eval(Board& board, sf::Color color) const;
		int getConnectedVal(Position before, Position after, int connected, Board& board) const;
		//int* convertBoard(Board& board) const;
};

#endif