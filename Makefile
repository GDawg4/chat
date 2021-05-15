compile:
	gcc -Wall -g3 -fsanitize=address -pthread server2.c -o server2
	gcc -Wall -g3 -fsanitize=address -pthread client2.c -o client2