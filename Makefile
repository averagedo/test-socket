all: client server 

client: client.c 
	g++ -Werror -o $@ client.c 

server: server.c 
	g++ -Werror -o $@ server.c 

remove:
	@rm -v client server
