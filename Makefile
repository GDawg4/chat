compile:
	sudo protoc --cpp_out=.new.proto
	# gcc -Wall -g3 -fsanitize=address -pthread -lprotobuf server3.c -o server3
	# gcc -Wall -g3 -fsanitize=address -pthread -lprotobuf client3.c -o client3
	gcc -o client3 client3.c new.pb-c.c -L/usr/local/lib -lprotobuf-c -lpthread	
	gcc -o server server.c new.pb-c.c -L/usr/local/lib -lprotobuf-c -lpthread	
	