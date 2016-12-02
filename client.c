#include <stdio.h>
#include <sys/socket.h> //socket
#include <arpa/inet.h> //inet_addr
#include <string.h> //strlen
#include <unistd.h> //close(socket_desc)

int main(){

    //create a socket, returns -1 of error happened
    /*
    Address Family - AF_INET (this is IP version 4)
    Type - SOCK_STREAM (this means connection oriented TCP protocol)
    Protocol - 0 [ or IPPROTO_IP This is IP protocol]
    */
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1) {
        printf("Could not create a socket\n");
    }

    //create a server
    struct sockaddr_in server;
    /*
    // IPv4 AF_INET sockets:
    struct sockaddr_in {
    short            sin_family;   // e.g. AF_INET, AF_INET6
    unsigned short   sin_port;     // e.g. htons(3490)
    struct in_addr   sin_addr;     // see struct in_addr, below
    char             sin_zero[8];  // zero this if you want to
    };

    struct in_addr {
    unsigned long s_addr;          // load with inet_pton()
    };

    struct sockaddr {
    unsigned short    sa_family;    // address family, AF_xxx
    char              sa_data[14];  // 14 bytes of protocol address
    };
    */

    char *IP = "127.0.0.1";
    unsigned short OPEN_PORT = 8888; //first, you may want to find the open port before typing dummy 8000

    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(OPEN_PORT); //specify the open port_number

    //connect to the server
    if (connect(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0) {
        puts("Connect error\n");
        return 1;
    }
    puts("Connected\n");

    //send some data
    char* message = "GET / HTTP/1.1\r\n\r\n";
    if (send(socket_desc, message, strlen(message), 0) < 0) {
        puts("Send failed\n");
        return 1;
    }
    puts("Data send\n");

    //receive data from server
    char server_reply[2000];
    while( recv(socket_desc, server_reply,2000,0) ) {
      puts("Reply received:\n");
      puts(server_reply);
    }
    puts("Receive failed\n");

    //close the socket PLEASE!!!
    //close(socket_desc);
    return 0;
}
