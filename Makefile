compile:
	sudo protoc --cpp_out=. new.proto

	# gcc -Wall -g3 -fsanitize=address -pthread -lprotobuf server3.c -o server3
	# gcc -Wall -g3 -fsanitize=address -pthread -lprotobuf client3.c -o client3
	gcc -o client3 client3.c new.pb-c.c -I/usr/local/include -lprotobuf-c -lpthread	
	gcc -o server3 server3.c new.pb-c.c -I/usr/local/include -lprotobuf-c -lpthread	
	