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
	unsigned score;
	unsigned max_scores[MAX_2N_SIZE+1];
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

void score_add (struct Game* game, unsigned value)
{
	game->score += value;
	if (game->score > game->max_scores[game->size])
		game->max_scores[game->size] = game->score;
}

bool is_game_over (struct Game* game)
{
	/* check for empty cells */
	for (unsigned j = 0;  j < game->size;  ++j) {
		for (unsigned i = 0;  i < game->size;  ++i) {
			if (game->board[j][i] == 0)
				return false;
		}
	}

	/* check for adjacent numbers */
	for (unsigned j = 0;  j < game->size-1;  ++j) {
		for (unsigned i = 0;  i < game->size-1;  ++i) {
			if (game->board[j][i] == game->board[j+1][i]
			||  game->board[j][i] == game->board[j][i+1])
				return false;
		}
		if (game->board[game->size-1][j] == game->board[game->size-1][j+1]
		||  game->board[j][game->size-1] == game->board[j+1][game->size-1])
			return false;
	}

	return true;
}

bool cell_has_equal_neighbour (struct Game* game, unsigned j, unsigned i)
{
	unsigned n = game->board[j][i];
	unsigned M = game->size-1;
	return
		(j < M && n == game->board[j+1][i]) ||
		(j > 0 && n == game->board[j-1][i]) ||
		(i < M && n == game->board[j][i+1]) ||
		(i > 0 && n == game->board[j][i-1]);
}

void draw (struct Game* game)
{
	static const char* colors[] = {
		"1;30", //empty
		"0;37", "1;37", "0;36", "1;36", // 2, 4, 8, 16
		"0;34", "1;34", "0;32", "1;32", // 32, 64, 128, 256
		"0;33", "1;33", "0;31", "1;31", // 512, 1024, 2048, 4096
		"0;35", "1;35", "1;35", "1;35"  // 8192...
	};

	printf("\e[1;1f\e[2J\e[0m");
	printf("\e[1;30mscore: \e[1;37m%d\e[1;30m, max_score: \e[1;37m%d\n",
			game->score,
			game->max_scores[game->size]
	);

	for (unsigned j = 0;  j < game->size;  ++j) {
		for (unsigned i = 0;  i < game->size;  ++i) {
			unsigned n = game->board[j][i];
			printf("\e[%sm", colors[n]);
			if (n == 0)
				printf("    . ");
			else {
				bool b = cell_has_equal_neighbour(game, j, i);
				printf("%c%4d%c", b?'[':' ', 1<<n, b?']':' ');
			}
		}
		printf("\n");
	}

	if (is_game_over(game)) {
		printf("\e[0;1;41m G A M E    O V E R \e[0m  (press N for new game)");
		fflush(stdout);
	}
}

void board_init (struct Game* game)
{
	game->size = game->new_size;
	game->score = 0;

	memset(game->board, 0, sizeof(game->board));

	add_random_number(game);
	add_random_number(game);
}

void game_save (struct Game* game)
{
	FILE* F = fopen(game->savefile, "w");
	if (F != NULL) {

		/* scores */
		fprintf(F, "%d\n", game->score);
		for (int size = MIN_2N_SIZE;  size <= MAX_2N_SIZE;  ++size) {
			fprintf(F, "%d=%d,", size, game->max_scores[size]);
		}
		fprintf(F, "\n");

		/* board size */
		fprintf(F, "%d\n", game->size);

		/* board content */
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

		/* scores */
		fscanf(F, "%d\n", &game->score);
		for (int size = MIN_2N_SIZE;  size <= MAX_2N_SIZE;  ++size) {
			int saved_size = 0;
			fscanf(F, "%d=%d,", &saved_size, &game->max_scores[size]);
			assert(saved_size == size);
		}
		fscanf(F, "\n");

		/* board size */
		fscanf(F, "%d\n", &game->size);

		/* board content */
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

	memset(game->max_scores, 0, sizeof(game->max_scores));

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

	/* used to accept wasd keys and translate to hjkl */
	const char* arrows = "khjlwasd";
	char* arrows_ptr;

	while(1) {
		char key = RawKb_GetChar();
		if (key == 'q') { /* quit */
			game_save(game);
			break;
		}
		else if (key == 'n') { /* new game */
			board_init(game);
		}
		else if ((arrows_ptr = strchr(arrows, key)) != NULL) {

			/* translate wasd to hjkl */
			if (arrows_ptr - arrows >= 4)
				key = *(arrows_ptr-4);

			bool moved = false;
			for (int idx = 0;  idx < game->size;  ++idx) {
				struct Itr the_itr;
				struct Itr* itr = &the_itr;

				/* move over empty spaces */
				itr_init(itr, game, key, idx);
				for (;  !itr_is_last(itr);  itr_move(itr)) {
					while (itr_get(itr) == 0) {
						if (itr_shift(*itr))
							moved = true;
						else
							break;
					}
				}

				/* collapse equals */
				itr_init(itr, game, key, idx);
				for (;  !itr_is_last(itr);  itr_move(itr)) {
					if (itr_get(itr) == itr_get_next(itr)  &&  itr_get(itr) != 0) {
						itr_set(itr, itr_get(itr)+1);
						score_add(game, 1 << itr_get(itr));
						struct Itr itr_copy = *itr;
						itr_move(&itr_copy);
						itr_shift(itr_copy);
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
