#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "rawkb.h"

#define ERROR assert("logic error"==0)

struct Game
{
	unsigned board[4][4];
};

struct Itr
{
	struct Game* game;
	unsigned j, i;
	char dir;
};

void itr_init (struct Itr* itr, struct Game* game, char dir, unsigned idx)
{
	itr->game = game;
	switch (itr->dir = dir) {
		case 'l':
			itr->j = idx;
			itr->i = 3;
			break;
		case 'h':
			itr->j = idx;
			itr->i = 0;
			break;
		default:
			ERROR;
	}
}

bool itr_is_last (struct Itr* itr)
{
	switch (itr->dir) {
		case 'l':
			return (itr->i == 0);
		case 'h':
			return (itr->i == 3);
	}
	ERROR;
}

bool itr_move (struct Itr* itr)
{
	if (itr_is_last(itr))
		return false;

	switch (itr->dir) {
		case 'l':
			--itr->i;
			return true;
		case 'h':
			++itr->i;
			return true;
	}
	ERROR;
}

void itr_set (struct Itr* itr, unsigned n)
{
	itr->game->board[itr->j][itr->i] = n;
}

unsigned itr_get (struct Itr* itr)
{
	return itr->game->board[itr->j][itr->i];
}

unsigned itr_get_next (struct Itr* itr)
{
	if (itr_is_last(itr))
		ERROR;

	switch (itr->dir) {
		case 'l':
			return itr->game->board[itr->j][itr->i-1];
		case 'h':
			return itr->game->board[itr->j][itr->i+1];
	}
	ERROR;
}

bool itr_shift (struct Itr the_itr)
{
	struct Itr* itr = &the_itr;

	bool all_zeros = true;
	for (;  !itr_is_last(itr);  itr_move(itr)) {
		itr_set(itr, itr_get_next(itr));
		if (itr_get(itr) != 0)
			all_zeros = false;
	}
	itr_set(itr, 0);
	return !all_zeros;
}

unsigned free_cell_count (struct Game* game)
{
	unsigned count = 0;

	for (unsigned j = 0;  j < 4;  ++j) {
		for (unsigned i = 0;  i < 4;  ++i) {
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

	for (unsigned j = 0;  j < 4;  ++j) {
		for (unsigned i = 0;  i < 4;  ++i) {
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

	ERROR;
}

void draw (struct Game* game)
{
	for (unsigned j = 0;  j < 4;  ++j) {
		for (unsigned i = 0;  i < 4;  ++i) {
			unsigned n = game->board[j][i];
			if (n == 0)
				printf("     .");
			else
				printf("  %4d", n);
		}
		printf("\n");
	}
}

void game_init (struct Game* game)
{
	memset(game, 0, sizeof(struct Game));

	add_random_number(game);
	add_random_number(game);
}

int main()
{
	srand(time(0));

	struct Game game_data;
	struct Game* game = &game_data;
	game_init(game);

	RawKb_Open(RAWKB_MODE_WAIT);

	draw(game);
	printf("\n");

	while(1) {
		char k = RawKb_GetChar();
		if (k == 'q') { /* quit */
			break;
		}
		else if (k == 'c') { /* clear board */
			game_init(game);
		}
		else if (k == 'l'  ||  k == 'h') {
			for (int j = 0;  j < 4;  ++j) {
				struct Itr the_itr;
				struct Itr* itr = &the_itr;

				itr_init(itr, game, k, j);
				for (;  !itr_is_last(itr);  itr_move(itr)) {
					while (itr_get(itr) == 0) {
						if (itr_shift(*itr) == false)
							break;
					}
				}

				itr_init(itr, game, k, j);
				for (;  !itr_is_last(itr);  itr_move(itr)) {
					if (itr_get(itr) == itr_get_next(itr)  &&  itr_get(itr) != 0) {
						itr_set(itr, itr_get(itr) * 2);
						itr_move(itr);
						itr_shift(*itr);
					}
				}
			}
		}
		else {
			add_random_number(game);
		}

		draw(game);
		printf("\n");
	}

	RawKb_Close();
	return 0;
}

// vim600:foldmethod=syntax:foldnestmax=1:
