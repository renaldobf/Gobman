CC=g++
CFLAGS=-Wall -O3
LDFLAGS=-s -static-libgcc -static-libstdc++
LDLIBS=-lallegro -lallegro_image -lallegro_font -lallegro_audio -lallegro_acodec -lallegro_primitives

all: build/base.o build/config.o build/control_menu.o build/credits.o build/help.o build/hidden_menu.o build/hiscore.o build/intro.o build/level/level.o build/level/load.o build/level/update_logics.o build/main.o build/main_menu.o build/setup.o 
	$(CC) $(CFLAGS) -o gobman build/base.o build/config.o build/control_menu.o build/credits.o build/help.o build/hidden_menu.o build/hiscore.o build/intro.o build/level/level.o build/level/load.o build/level/update_logics.o build/main.o build/main_menu.o build/setup.o  $(LDFLAGS) $(LDLIBS)

build/base.o: src/base.cpp src/base.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/base.cpp -o build/base.o

build/config.o: src/config.cpp src/config.h src/base.h src/resources.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/config.cpp -o build/config.o

build/control_menu.o: src/control_menu.cpp src/control_menu.h src/base.h src/resources.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/control_menu.cpp -o build/control_menu.o

build/credits.o: src/credits.cpp src/credits.h src/base.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/credits.cpp -o build/credits.o

build/help.o: src/help.cpp src/help.h src/base.h src/resources.h src/level/level.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/help.cpp -o build/help.o

build/hidden_menu.o: src/hidden_menu.cpp src/hidden_menu.h src/base.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/hidden_menu.cpp -o build/hidden_menu.o

build/hiscore.o: src/hiscore.cpp src/hiscore.h src/base.h src/resources.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/hiscore.cpp -o build/hiscore.o

build/intro.o: src/intro.cpp src/intro.h src/base.h src/resources.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/intro.cpp -o build/intro.o

build/level/level.o: src/level/level.cpp src/level/level.h src/level/load.h src/level/update_logics.h src/hiscore.h 
	@ mkdir -p build/level
	$(CC) $(CFLAGS) -c src/level/level.cpp -o build/level/level.o

build/level/load.o: src/level/load.cpp src/level/load.h src/level/level.h src/base.h src/resources.h 
	@ mkdir -p build/level
	$(CC) $(CFLAGS) -c src/level/load.cpp -o build/level/load.o

build/level/update_logics.o: src/level/update_logics.cpp src/level/level.h src/level/update_logics.h 
	@ mkdir -p build/level
	$(CC) $(CFLAGS) -c src/level/update_logics.cpp -o build/level/update_logics.o

build/main.o: src/main.cpp src/base.h src/config.h src/setup.h src/main_menu.h src/intro.h src/credits.h src/hiscore.h src/help.h src/level/level.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/main.cpp -o build/main.o

build/main_menu.o: src/main_menu.cpp src/main_menu.h src/base.h src/resources.h src/control_menu.h src/hidden_menu.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/main_menu.cpp -o build/main_menu.o

build/setup.o: src/setup.cpp src/setup.h src/base.h src/resources.h 
	@ mkdir -p build
	$(CC) $(CFLAGS) -c src/setup.cpp -o build/setup.o

clean:
	rm -rf -- build/* gobman

install: gobman
	./install

.PHONY: install

