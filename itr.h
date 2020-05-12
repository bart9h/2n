#ifndef itr_h_included
#define itr_h_included

#include <stdbool.h>
#include "game.h"

struct Itr
{
	struct Game* game;
	unsigned j, i;
	char direction;
};

void itr_init (struct Itr*, struct Game*, char direction, unsigned index);
bool itr_is_last (struct Itr*);
bool itr_move (struct Itr*);
void itr_set (struct Itr*, unsigned value);
unsigned itr_get (struct Itr*);
unsigned itr_get_next (struct Itr*);
bool itr_shift (struct Itr);

#endif
