#ifndef COMPUTER_PLAYER_H
#define COMPUTER_PLAYER_H

#include "Board.h"

class ComputerPlayer {
	public:
		ComputerPlayer();
		ComputerPlayer(sf::Color c);

		void setColor(sf::Color c);
		Position makeMove(Board& board) const;
	private:
		sf::Color color;
};

#endif