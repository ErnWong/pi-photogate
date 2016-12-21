program: main.c
	gcc -std=gnu99 -lm -lwiringPi -Wall -Wextra -o $@ $^
