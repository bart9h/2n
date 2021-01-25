
# 2n

"2048" clone for the console

http://github.com/bart9h/2n


## Installation

Just run `make` to compile.
There are no dependencies beyond GNU make and gcc.


## How to play

Use `w`, `a`, `s`, `d` or vi arrow keys (`h`, `j`, `k`, `l`) to play.

`m` to cycle through draw modes (fancy, compact and ultra-compact).

`n` resets the board (new game).

`q` quits.

Game state is automatically saved/loaded in `~/.config/2nrc`.

Set environment variable `SIZE` to change the board size
(effective on next board init), must be between 3 and 16.

Example: `SIZE=8 ./2n`


## Navigating the source code:

* 2n.c: main() and drawing

* game.c, game.h: game logic

* itr.c, itr.h: cell iterator

* rawkb.c, rawkb.h: get input from the keyboard
