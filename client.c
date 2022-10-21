// Client side C/C++ program to demonstrate Socket
// programming
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <iostream>
#include <ios>
#define PORT 10000

int sendmess(int in) {
	int sock = 0, valread, client_fd;
	struct sockaddr_in serv_addr;
	char data[1000];
	char buffer[1024] = { 0 };
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("\n Socket creation error \n");
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);

	// Convert IPv4 and IPv6 addresses from text to binary
	// form
	if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<= 0) {
		printf(
			"\nInvalid address/ Address not supported \n");
		return -1;
	}

	if ((client_fd= connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))< 0) {
		printf("\nConnection Failed \n");
		return -1;
	}

	for(int i=0;i<100;i++) {
		sprintf(data,"Client %d ,request %d\n", in, i);
		send(sock, data, strlen(data), 0);
		printf("%s\n",data);
		sleep(1);
		// valread = read(sock, buffer, 1024);
		// printf("%s\n", buffer);
	}
	// closing the connected socket
	close(client_fd);
	return 0;
}

int main(int argc, char const* argv[])
{
	std::vector<std::thread> vec_thr;

	for (int i=0; i< 10000; i++) {
		vec_thr.push_back(std::thread(sendmess, i));
	}
	
	
	for (int i=0; i< vec_thr.size(); i++) {
		vec_thr[i].join();
	}
}
