#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "game.h"
#include "itr.h"
#include "rawkb.h"

void draw_line (struct Game* game)
{
	printf("\e[1;30m+");
	for (unsigned i = 0;  i < game->size;  ++i)
		printf("------+");
	printf("\n");
}

void draw_space (struct Game* game)
{
	printf("\e[1;30m|");
	for (unsigned i = 0;  i < game->size;  ++i)
		printf("      |");
	printf("\n");
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
	printf("\e[1;30m%s\e[1;37m%d\e[1;30m, %s\e[1;37m%d\n",
			game->draw_mode ? "score: " : "s",
			game->score,
			game->draw_mode ? "max_score: " : "m",
			game->max_scores[game->size]
	);

	for (unsigned j = 0;  j < game->size;  ++j) {

		if (game->draw_mode == 2) {
			draw_line(game);
			draw_space(game);
			printf("|");
		}

		for (unsigned i = 0;  i < game->size;  ++i) {
			unsigned n = game->board[j][i];
			printf("\e[%sm", colors[n]);
			if (n == 0)
				printf("%s . ", game->draw_mode ? "   " : "");
			else {
				bool b = cell_has_equal_neighbour(game, j, i);
				printf("%c", b?'[':' ');
				if (game->draw_mode)
					printf("%4d", 1<<n);
				else
					printf("%x", n-1);
				printf("%c", b?']':' ');
			}

			if (game->draw_mode == 2)
				printf("\e[1;30m|");
		}
		printf("\n");
	}

	if (game->draw_mode == 2)
		draw_line(game);

	if (is_game_over(game)) {
		if (game->draw_mode)
			printf("\e[0;1;41m G A M E    O V E R \e[0m  (press N for new game)");
		else
			printf(" \e[0;31m_x__x__x_\e[0m ");
		fflush(stdout);
	}
}

int main()
{
	srand(time(0));

	struct Game game_data;
	struct Game* game = &game_data;

	if (!game_init(game))
		return 1;

	/* board size config for new boards */
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

	/* location of the save file */
	char* config_dir_env = getenv("XDG_CONFIG_HOME");
	if (config_dir_env != NULL)
		sprintf(game->savefile, "%s/2n", config_dir_env);
	else
		sprintf(game->savefile, "%s/.config/2n", getenv("HOME"));

	game_load(game);

	RawKb_Open(RAWKB_MODE_WAIT);

	draw(game);

	while(1) {
		char key = RawKb_GetChar();
		if (key == 'q') { /* quit */
			game_save(game);
			printf("\n");
			break;
		}
		else {
			game_process_input (game, key);
		}

		draw(game);
	}

	RawKb_Close();
	return 0;
}

// vim600:foldmethod=syntax:foldnestmax=1:
