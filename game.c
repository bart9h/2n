#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"
#include "itr.h"

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
{// return a random integer from 0 to n-1

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

	LOGIC_ERROR;
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

		/* draw_mode mode */
		fprintf(F, "%d\n", game->draw_mode);

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

		/* draw_mode mode */
		fscanf(F, "%d\n", &game->draw_mode);

		fclose(F);
	}
}

bool game_init (struct Game* game)
{
	game->draw_mode = 1;
	game->new_size = 4;

	memset(game->max_scores, 0, sizeof(game->max_scores));

	board_init(game);

	return true;
}

void game_rotate (struct Game* game)
{
	struct Game copy = *game;
	int N = game->size - 1;

	for (int i = 0;  i <= N;  ++i)
		for (int j = 0;  j <= N;  ++j)
			game->board[j][i] = copy.board[N-i][j];
}

void game_hflip (struct Game* game)
{
	struct Game copy = *game;
	int N = game->size - 1;

	for (int i = 0;  i <= N;  ++i)
		for (int j = 0;  j <= N;  ++j)
			game->board[j][i] = copy.board[j][N-i];
}

void game_vflip (struct Game* game)
{
	struct Game copy = *game;
	int N = game->size - 1;

	for (int i = 0;  i <= N;  ++i)
		for (int j = 0;  j <= N;  ++j)
			game->board[j][i] = copy.board[N-j][i];
}

void game_process_input (struct Game* game, unsigned char key)
{
	/* used to accept wasd keys and translate to hjkl */
	const char* arrows = "khjlwasd";
	char* arrows_ptr;

	if (key == 'n') { /* new game */
		if (is_game_over(game)) {
			board_init(game);
		}
		else {
			printf("reset game? ");
			if (getchar() == 'y')
				board_init(game);
		}
	}
	else if (key == 'm') { /* draw modes */
		game->draw_mode = (game->draw_mode+1)%3;
	}
	else if (key == 'r') { /* rotation */
		game_rotate (game);
	}
	else if (key == 'f') { /* horizontal flip */
		game_hflip (game);
	}
	else if (key == 'v') { /* vertical flip */
		game_vflip (game);
	}
	else if ((arrows_ptr = strchr(arrows, key)) != NULL) {

		/* translate wasd to hjkl */
		if (arrows_ptr - arrows >= 4)
			key = *(arrows_ptr-4);

		bool moved = false;
		for (int index = 0;  index < game->size;  ++index) {
			struct Itr the_itr;
			struct Itr* itr = &the_itr;

			/* move over empty spaces */
			itr_init(itr, game, key, index);
			for (;  !itr_is_last(itr);  itr_move(itr)) {
				while (itr_get(itr) == 0) {
					if (itr_shift(*itr))
						moved = true;
					else
						break;
				}
			}

			/* collapse equals */
			itr_init(itr, game, key, index);
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

		if (moved) {
			add_random_number(game);
			game_save(game);
		}
	}
}

// vim600:foldmethod=syntax:foldnestmax=1:
