# Gobman

This is a remake of Gobman, a shareware Pac-Man clone made by Filipe Mateus and released in 1992.

From the original distribution:

> Gobman is a pacman clone with a few nice twists. Collect bombs, hourglasses, and special red dot to use against the monsters. Nice graphics and good game play.

The main reason to make this remake was because the original game glitches and becomes unplayable on DOSBox after a few minutes.

## Dependencies

It depends on [Allegro 5](http://liballeg.org/index.html) library.

On Debian/Ubuntu, you can get it from the apt repositories.

	apt-get install liballegro5-dev liballegro-image5-dev liballegro-audio5-dev liballegro-acodec5-dev

## Building

To build, just invoke `make`.

	make

If you modify the source code structure, you need to run `configure` first.

	./configure
	make

Optionally, you can install it to the machine, just run:

	make
	make install
