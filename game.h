#ifndef game_h_included
#define game_h_included

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

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
	unsigned draw_mode;
};

bool cell_has_equal_neighbour (struct Game*, unsigned j, unsigned i);
bool is_game_over (struct Game*);
bool game_init (struct Game*);
void game_load (struct Game*);
void game_save (struct Game*);
void game_process_input (struct Game*, unsigned char key);

#endif
