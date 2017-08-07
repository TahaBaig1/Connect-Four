#include "Game.h"

int main() {
	bool playGame = true;
	while (playGame) {
		Game game;
		playGame = game.run();
	}
	return 0;
}