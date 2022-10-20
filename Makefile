all: client server 

client: client.c 
	gcc -Werror -o $@ client.c 

server: server.c 
	gcc -Werror -o $@ server.c 

remove:
	@rm -v client server
