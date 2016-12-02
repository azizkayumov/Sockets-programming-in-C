#include <stdio.h>
#include <sys/socket.h> //socket
#include <arpa/inet.h> //inet_addr
#include <unistd.h> //write(socket, message, strlen(message))
#include <string.h> //strlen
#include <stdlib.h> //strlen

#include <pthread.h> //for threading, link with lpthread:
/*
      gcc program.c -lpthread
*/


//connection handler
void *connection_handler(void *);

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

      if ( bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0 ) {
            puts("Bind failed\n");
            return 1;
      }
      puts("Bind done");

      //listen for new connections:
      listen(socket_desc, 3);
      puts("Waiting for new connections...");

      int c = sizeof(struct sockaddr_in);
      //client to be connected
      struct sockaddr_in client;
      //new socket for client
      int new_socket, *new_sock;
      while (   (new_socket = accept( socket_desc, (struct sockaddr*)&client, (socklen_t*)&c ))   ) {
        puts("Connection accepted");
        //get the IP address of a client
        char *CLIENT_IP = inet_ntoa(client.sin_addr);
        int CLIENT_PORT = ntohs(client.sin_port);
        printf("       CLIENT = {%s:%d}\n", CLIENT_IP, CLIENT_PORT);
        //reply to the client
        char* message = "Hello, client! I have received your connection, now I will assign a handler for you!\n";
        write(new_socket, message, strlen(message));

        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;

        if(   pthread_create(&sniffer_thread, NULL, connection_handler, (void*)new_sock) < 0  ){
            perror("could not create thread");
            return 1;
        }
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
        puts("       Handler assigned");
      }
      if ( new_socket<0 ) {
          perror("Accept failed");
          return 1;
      }

      return 0;
}



//connection handler
void *connection_handler(void *socket_desc){
      //get the socket descriptor
      int sock = *(int*)socket_desc;
      int read_size;
      char* message, client_message[2000];
      //send some message to the client
      message = "Greetings, I am your connection handler\n";
      write(sock, message, strlen(message));

      message = "Now type something and i shall repeat what you type \n";
      write(sock , message , strlen(message));

      //Receive a message from client
      while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 ){
          //Send the message back to client
          message = "You wrote: ";
          //Send the message back to client
          write(sock , message , strlen(message));
          write(sock , client_message , read_size);
          //bzero(client_message, read_size);
      }

      if(read_size == 0){
          puts("Client disconnected");
          fflush(stdout);
      }
      else if(read_size == -1){
          perror("recv failed");
      }

      //Free the socket pointer
      free(socket_desc);
      return 0;
}
