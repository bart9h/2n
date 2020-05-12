TARGET = 2n

OBJS = 2n.o game.o itr.o rawkb.o

CFLAGS = -Wall -Werror -std=c99

$(TARGET): $(OBJS)

2n.o: game.h itr.h
game.o: game.h itr.h
itr.o: itr.h

clean:
	rm -f $(OBJS)
