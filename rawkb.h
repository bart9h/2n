#ifndef rawkb_h_included
#define rawkb_h_included

#include <stdbool.h>

typedef enum {
	RAWKB_MODE_WAIT,
	RAWKB_MODE_NOWAIT
} rawkb_mode_t;

bool RawKb_Open (rawkb_mode_t mode);
char RawKb_GetChar();
bool RawKb_Close();

#endif
