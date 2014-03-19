#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "rawkb.h"

#define ERROR assert("logic error"==0)
#define MAX_2N_SIZE 16
#define MIN_2N_SIZE 3

struct Game
{
	unsigned size;
	unsigned new_size;
	unsigned board[MAX_2N_SIZE][MAX_2N_SIZE];
	char savefile[FILENAME_MAX];
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
			itr->i = game->size-1;
			break;
		case 'h':
			itr->j = idx;
			itr->i = 0;
			break;
		case 'j':
			itr->j = game->size-1;
			itr->i = idx;
			break;
		case 'k':
			itr->j = 0;
			itr->i = idx;
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
			return (itr->i == itr->game->size-1);
		case 'j':
			return (itr->j == 0);
		case 'k':
			return (itr->j == itr->game->size-1);
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
		case 'j':
			return --itr->j;
		case 'k':
			return ++itr->j;
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
		case 'j':
			return itr->game->board[itr->j-1][itr->i];
		case 'k':
			return itr->game->board[itr->j+1][itr->i];
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

	for (unsigned j = 0;  j < game->size;  ++j) {
		for (unsigned i = 0;  i < game->size;  ++i) {
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

	for (unsigned j = 0;  j < game->size;  ++j) {
		for (unsigned i = 0;  i < game->size;  ++i) {
			if (game->board[j][i] == 0) {
				if (free_idx > 0) {
					--free_idx;
				}
				else {
					game->board[j][i] = random_int(10) < 9 ? 1 : 2;
					return true;
				}
			}
		}
	}

	ERROR;
}

void draw (struct Game* game)
{
	static const char* colors[] = {
		"1;30", //empty
		"0;37", "1;37", "0;36", "1;36", // 2, 4, 8, 16
		"0;34", "1;34", "0;32", "1;32", // 32, 64, 128, 256
		"0;33", "1;33", "0;31", "1;31", // 512, 1024, 2048, 4096
		"1;35", "1;45", NULL // 8192, !!!!!!
	};

	printf("\e[1;1f\e[2J");
	for (unsigned j = 0;  j < game->size;  ++j) {
		for (unsigned i = 0;  i < game->size;  ++i) {
			unsigned n = game->board[j][i];
			printf("\e[%sm", colors[n]);
			if (n == 0)
				printf("     .");
			else
				printf("  %4d", 1<<n);
		}
		printf("\n");
	}
}

void board_init (struct Game* game)
{
	game->size = game->new_size;

	memset(game->board, 0, sizeof(game->board));

	add_random_number(game);
	add_random_number(game);
}

void game_save (struct Game* game)
{
	FILE* F = fopen(game->savefile, "w");
	if (F != NULL) {
		fprintf(F, "%d\n", game->size);
		for (int j = 0;  j < game->size;  ++j) {
			for (int i = 0;  i < game->size;  ++i) {
				fprintf(F, "%d,", game->board[j][i]);
			}
			fprintf(F, "\n");
		}
		fclose(F);
	}
}

void game_load (struct Game* game)
{
	FILE* F = fopen(game->savefile, "r");
	if (F != NULL) {
		fscanf(F, "%d\n", &game->size);
		for (int j = 0;  j < game->size;  ++j) {
			for (int i = 0;  i < game->size;  ++i) {
				fscanf(F, "%d,", &game->board[j][i]);
			}
			fscanf(F, "\n");
		}
		fclose(F);
	}
}

bool game_init (struct Game* game)
{
	game->new_size = 4;

	char* size_env = getenv("SIZE");
	if (size_env != NULL) {
		int size_arg = atoi(size_env);
		if (size_arg < MIN_2N_SIZE || size_arg > MAX_2N_SIZE) {
			fprintf(stderr, "size must be >= 3 and <= 16\n");
			return false;
		}
		else {
			game->new_size = size_arg;
		}
	}

	board_init(game);
	return true;
}

int main()
{
	srand(time(0));

	struct Game game_data;
	struct Game* game = &game_data;
	sprintf(game->savefile, "%s/.2nrc", getenv("HOME"));

	if (!game_init(game))
		return 1;

	game_load(game);

	RawKb_Open(RAWKB_MODE_WAIT);

	draw(game);
	printf("\n");

	while(1) {
		char key = RawKb_GetChar();
		if (key == 'q') { /* quit */
			game_save(game);
			break;
		}
		else if (key == 'c') { /* clear board */
			board_init(game);
		}
		else if (key == 'l'  ||  key == 'h'  ||  key == 'k'  ||  key == 'j') {
			bool moved = false;
			for (int idx = 0;  idx < game->size;  ++idx) {
				struct Itr the_itr;
				struct Itr* itr = &the_itr;

				itr_init(itr, game, key, idx);
				for (;  !itr_is_last(itr);  itr_move(itr)) {
					while (itr_get(itr) == 0) {
						if (itr_shift(*itr))
							moved = true;
						else
							break;
					}
				}

				itr_init(itr, game, key, idx);
				for (;  !itr_is_last(itr);  itr_move(itr)) {
					if (itr_get(itr) == itr_get_next(itr)  &&  itr_get(itr) != 0) {
						itr_set(itr, itr_get(itr)+1);
						itr_move(itr);
						itr_shift(*itr);
						moved = true;
					}
				}
			}

			if (moved)
				add_random_number(game);
		}

		draw(game);
	}

	RawKb_Close();
	return 0;
}

// vim600:foldmethod=syntax:foldnestmax=1:
