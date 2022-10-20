#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<sys/epoll.h>
#include<signal.h>

#define PORT 10000
#define MAX_EVENT 10

struct epoll_event ev, events[MAX_EVENT];
int running=1;

void sig_handler(int signum) {
    printf("\nStop program !!!\n");
    running=0;
}

int main(int argc, char const* argv[]) {
    int server_fd, 
        new_socket, 
        epollfd,
        nfds ;
    struct sockaddr_in address;
    int opt=1;
    int addrlen=sizeof(address);
    char buffer[1024]={0};
    char* hello= "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 10000
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 10000
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) <0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Create epoll
    epollfd=epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd=server_fd;
    if (epoll_ctl(epollfd,EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl");
        exit(EXIT_FAILURE);
    }

    // Signal handle
    signal(SIGINT,sig_handler);

    while(running) {
        nfds = epoll_wait(epollfd, events, MAX_EVENT, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (int n=0; n< nfds; n++) {
            if (events[n].data.fd == server_fd) {
                new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                if (new_socket == -1) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }
                printf("Accept new df: %d\n",new_socket);
                ev.events=EPOLLIN | EPOLLET;
                ev.data.fd=new_socket;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, new_socket, &ev) == -1) {
                    perror("epoll_ctl: new_socket");
                    exit(EXIT_FAILURE);
                }
            } else {
                int valread;
                valread = read(events[n].data.fd, buffer, 1024);
                printf("FD number: %d, EVENT: %d\n", events[n].data.fd, events[n].events);
                printf("%s\n",buffer);

            }
        }
    }


    // if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
    //     perror("accept");
    //     exit(EXIT_FAILURE);
    // }

    // send(new_socket, hello, strlen(hello), 0);
    // printf("Hello message sent\n");

    // closing the connected socket
    //close(new_socket);
    // closing the listening socket
    close(server_fd);
    return 0;
}