#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "rawkb.h"

struct Game
{
	unsigned board[4][4];
};

unsigned free_cell_count (struct Game* game)
{
	unsigned count = 0;

	for (unsigned j = 0; j < 4; ++j) {
		for (unsigned i = 0; i < 4; ++i) {
			if (game->board[j][i] == 0)
				++count;
		}
	}

	return count;
}

unsigned random_int (unsigned n)
{
	return (int) (n * (rand() / (RAND_MAX + 1.0)));
}

bool add_random_number (struct Game* game)
{
	unsigned free_count = free_cell_count(game);
	if (free_count == 0)
		return false;

	unsigned free_idx = random_int(free_count);

	for (unsigned j = 0; j < 4; ++j) {
		for (unsigned i = 0; i < 4; ++i) {
			if (game->board[j][i] == 0) {
				if (free_idx > 0) {
					--free_idx;
				}
				else {
					game->board[j][i] = random_int(10) < 9 ? 2 : 4;
					return true;
				}
			}
		}
	}

	assert("logic error"==0);
}

void draw (struct Game* game)
{
	for (unsigned j = 0; j < 4; ++j) {
		for (unsigned i = 0; i < 4; ++i) {
			unsigned n = game->board[j][i];
			if (n == 0)
				printf("     .");
			else
				printf("  %4d", n);
		}
		printf("\n");
	}
}

int main()
{
	struct Game game;
	memset(&game, 0, sizeof(game));

	srand(time(0));

	add_random_number(&game);
	add_random_number(&game);

	RawKb_Open(RAWKB_MODE_WAIT);

	draw(&game);
	printf("\n");

	while(1) {
		char k = RawKb_GetChar();
		if (k == 'q') {
			break;
		}

		add_random_number(&game);
		draw(&game);
		printf("\n");
	}

	RawKb_Close();
	return 0;
}

// vim600:foldmethod=syntax:foldnestmax=1:
