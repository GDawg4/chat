compile:
	sudo protoc --c_out=. new.proto --my_codegen_out=.

	# gcc -Wall -g3 -fsanitize=address -pthread -lprotobuf server3.c -o server3
	# gcc -Wall -g3 -fsanitize=address -pthread -lprotobuf client3.c -o client3
	gcc -o client3 client3.c new.pb-c.c -L/usr/local/lib -lprotobuf-c -lpthread	
	gcc -o server3 server3.c new.pb-c.c -L/usr/local/lib -lprotobuf-c -lpthread	
	