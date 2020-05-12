#include "rawkb.h"
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <sys/time.h>

static struct {
	rawkb_mode_t mode;
	struct termios tc_original;
}
rawkb_data;

bool RawKb_Open (rawkb_mode_t mode)
{
	rawkb_data.mode = mode;

	// read current state
	if(tcgetattr(STDIN_FILENO, &rawkb_data.tc_original) == -1) {
		perror("tcgetattr");
		return false;
	}

	struct termios tc;
	tc = rawkb_data.tc_original;

	// don't wait for <enter>
	tc.c_lflag &= ~ICANON;

	// don't echo
	tc.c_lflag &= ~ECHO;

	if (rawkb_data.mode == RAWKB_MODE_NOWAIT) {
		tc.c_cc[VTIME] = 0;
		tc.c_cc[VMIN]  = 0;
	}

	if(tcsetattr(STDIN_FILENO, TCSANOW, &tc) == -1) {
		perror("tcsetattr");
		return false;
	}

	return true;
}

char RawKb_GetChar()
{
	if (rawkb_data.mode == RAWKB_MODE_NOWAIT) {
		struct timeval tv;

		fd_set fds;
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		tv.tv_sec = 0; tv.tv_usec = 1;
		if(select(STDIN_FILENO+1, &fds, NULL, NULL, &tv) <= 0)
			return 0;
		if(!FD_ISSET(STDIN_FILENO, &fds))
			return 0;
	}

	char key;

	if(read(STDIN_FILENO, &key, sizeof(char)) != sizeof(char)) {
		perror("read");
		return 0;
	}

	return key;
}

bool RawKb_Close()
{
	if(tcsetattr(STDIN_FILENO, TCSANOW, &rawkb_data.tc_original) == -1) {
		perror("tcsetattr");
		return false;
	}
	return true;
}

// vim600:foldmethod=syntax:foldnestmax=1:
