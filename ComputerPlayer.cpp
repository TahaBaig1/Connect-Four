#include "ComputerPlayer.h"
#include <cstdlib>

ComputerPlayer::ComputerPlayer() {}

ComputerPlayer::ComputerPlayer(sf::Color c) : color(c) {}

void ComputerPlayer::setColor(sf::Color c) {
	color = c;
}

//determines optimal move and returns column of where to place next piece
Position ComputerPlayer::makeMove(Board& board) const{
	
	Position placed(-1, -1);
	while (placed.x == -1 || placed.y == -1) {
		int col = rand() % board.getWidth();
		placed = board.addPiece(col, color);
	}

	return placed;
}
