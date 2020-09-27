#include <stdbool.h>
#include "game.h"
#include "itr.h"

void itr_init (struct Itr* itr, struct Game* game, char direction, unsigned index)
{
	itr->game = game;
	switch (itr->direction = direction) {
		case 'l':
			itr->j = index;
			itr->i = game->size-1;
			break;
		case 'h':
			itr->j = index;
			itr->i = 0;
			break;
		case 'j':
			itr->j = game->size-1;
			itr->i = index;
			break;
		case 'k':
			itr->j = 0;
			itr->i = index;
			break;
		default:
			LOGIC_ERROR;
	}
}

bool itr_is_last (struct Itr* itr)
{
	switch (itr->direction) {
		case 'l':
			return (itr->i == 0);
		case 'h':
			return (itr->i == itr->game->size-1);
		case 'j':
			return (itr->j == 0);
		case 'k':
			return (itr->j == itr->game->size-1);
	}
	LOGIC_ERROR;
}

bool itr_move (struct Itr* itr)
{
	if (itr_is_last(itr))
		return false;

	switch (itr->direction) {
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
	LOGIC_ERROR;
}

void itr_set (struct Itr* itr, unsigned value)
{
	itr->game->board[itr->j][itr->i] = value;
}

unsigned itr_get (struct Itr* itr)
{
	return itr->game->board[itr->j][itr->i];
}

unsigned itr_get_next (struct Itr* itr)
{
	if (itr_is_last(itr))
		LOGIC_ERROR;

	switch (itr->direction) {
		case 'l':
			return itr->game->board[itr->j][itr->i-1];
		case 'h':
			return itr->game->board[itr->j][itr->i+1];
		case 'j':
			return itr->game->board[itr->j-1][itr->i];
		case 'k':
			return itr->game->board[itr->j+1][itr->i];
	}
	LOGIC_ERROR;
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

// vim600:foldmethod=syntax:foldnestmax=1:
