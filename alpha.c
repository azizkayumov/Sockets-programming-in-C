  #include <stdio.h>
  #include <sys/socket.h> //socket
  #include <arpa/inet.h> //inet_addr
  #include <unistd.h> //write(socket, message, strlen(message))
  #include <string.h> //strlen
  #include <stdlib.h> //strlen and strcmp
  #include <time.h> //srand
  #include <pthread.h> //for threading, link with lpthread:

  /*
      gcc program.c -lpthread
  */

  //connection handler
  void *connection_handler(void *);

  char* card_number1="0000 0000 0000 0000";
  unsigned int balance1 = 1000;
  char* card_number2="1000 0000 0000 0000";
  unsigned int balance2 = 1500;

  //clear screen
  void clrscr(){
    system("cls||clear");
  }

  int main(){
      //create a socket, returns -1 of error happened
      /*
      Address Family - AF_INET (this is IP version 4)
      Type - SOCK_STREAM (this means connection oriented TCP protocol)
      Protocol - 0 [ or IPPROTO_IP This is IP protocol]
      */

      srand(time(NULL)); //random seed
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
      int OPEN_PORT = 1000 + rand()%9000; //first, you may want to find the open port before typing dummy 8000

      server.sin_addr.s_addr = inet_addr(IP);
      server.sin_family = AF_INET;
      server.sin_port = htons(OPEN_PORT); //specify the open port_number

      if ( bind(socket_desc, (struct sockaddr*)&server, sizeof(server)) < 0 ) {
            puts("Bind failed\n");
            return 1;
      }
      puts("Bind done");
      printf("IP = [%s:%d]\n", IP, OPEN_PORT);

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
        //char* message = "Hello, client! I have received your connection, now I will assign a handler for you!\n";
        //write(new_socket, message, strlen(message));

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



  /*
    _    _       _           ____              _
   / \  | |_ __ | |__   __ _| __ )  __ _ _ __ | | __
  / _ \ | | '_ \| '_ \ / _` |  _ \ / _` | '_ \| |/ /
  / ___ \| | |_) | | | | (_| | |_) | (_| | | | |   <
  /_/   \_\_| .__/|_| |_|\__,_|____/ \__,_|_| |_|_|\_\
          |_|
  */
  //connection handler
  void *connection_handler(void *socket_desc){
      //get the socket descriptor
      int sock = *(int*)socket_desc;
      int read_size;
      char* message, client_message[2000];
      //send some art to the client to enjoy :-)
      message = "    _    _       _           ____              _\n";write(sock, message, strlen(message));
      message = "   / \\  | |_ __ | |__   __ _| __ )  __ _ _ __ | | __\n";write(sock, message, strlen(message));
      message = "  / _ \\ | | '_ \\| '_ \\ / _` |  _ \\ / _` | '_ \\| |/ /\n";write(sock, message, strlen(message));
      message = " / ___ \\| | |_) | | | | (_| | |_) | (_| | | | |   <\n";write(sock, message, strlen(message));
      message = "/_/   \\_\\_| .__/|_| |_|\\__,_|____/ \\__,_|_| |_|_|\\_\\\n";write(sock, message, strlen(message));
      message = "          |_|\n";write(sock, message, strlen(message));
      message = "--------------Welcome to AlphaBank ATM--------------\n";write(sock, message, strlen(message));
      message = "\n";write(sock, message, strlen(message));

  CONFIRM:
      //todo: enter visa card and its pin

  MENU: message = "1. Balance\n2. Deposit\n3. Withdraw\n4. Transfer\nPlease, choose the transaction number: ";
      write(sock , message , strlen(message));
      read_size = recv(sock, client_message, 2000, 0);
      switch (client_message[0]) {
          case '1':goto BALANCE;break;
          case '2':goto DEPOSIT;break;
          case '3':goto WITHDRAW;break;
          case '4':goto TRANSFER;break;
      }

  BALANCE:
      message = "------------------------------------------------------\n";
      write(sock, message, strlen(message));
      message = "Please, enter your card number: ";
      write(sock, message, strlen(message));
      read_size = recv(sock, client_message, 2000, 0);

      int isEqual = 1;
      for (size_t i = 0; i < strlen(card_number1); i++) {
         if (card_number1[i]!=client_message[i]) {
            isEqual = 0;
            break;
         }
      }

      if(isEqual==0){
        isEqual = 2;
        for (size_t i = 0; i < strlen(card_number2); i++) {
          if (card_number2[i]!=client_message[i]) {
              isEqual = 0;
              break;
          }
        }
      }

      if ( isEqual == 0 ) {
        message = "No such card number in our bank\n";
        write(sock, message, strlen(message));
      }else{

        if (isEqual==1) {
            char cated[2000];
            sprintf(cated, "Balance: %d USD\n", balance1);
            write(sock, cated, strlen(cated));
            message = "------------------------------------------------------\n";
            write(sock, message, strlen(message));
        }else
        if (isEqual==2) {
            char cated[2000];
            sprintf(cated, "Balance: %d USD\n", balance2);
            write(sock, cated, strlen(cated));
            message = "------------------------------------------------------\n";
            write(sock, message, strlen(message));
        }


      }

      message = "Enter 1 to go back to menu, 0 to exit: ";
      write(sock, message, strlen(message));
      read_size = recv(sock, client_message, 2000, 0);
      if (client_message[0]=='1') {
          goto MENU;
      }else
          goto EXIT;
  DEPOSIT:
      message = "------------------------------------------------------\n";
      write(sock, message, strlen(message));
      message = "Please, enter your card number: ";
      write(sock, message, strlen(message));
      read_size = recv(sock, client_message, 2000, 0);

      isEqual = 1;
      for (size_t i = 0; i < strlen(card_number1); i++) {
         if (card_number1[i]!=client_message[i]) {
            isEqual = 0;
            break;
         }
      }

      if(isEqual==0){
        isEqual = 2;
        for (size_t i = 0; i < strlen(card_number2); i++) {
          if (card_number2[i]!=client_message[i]) {
              isEqual = 0;
              break;
          }
        }
      }

      if ( isEqual == 0 ) {
        message = "No such card number in our bank\n";
        write(sock, message, strlen(message));
      }else{

        if (isEqual==1) {
          message = "Please, enter the amount of money to deposit: ";
          write(sock, message, strlen(message));
          read_size = recv(sock, client_message, 2000, 0);
          int cash = atoi(client_message);
          balance1 += cash;
          message = "Please, insert cash to the ATM...\nDone!\n";
          write(sock, message, strlen(message));
          message = "------------------------------------------------------\n";
          write(sock, message, strlen(message));
        }else
        if (isEqual==2) {
          message = "Please, enter the amount of money to deposit: ";
          write(sock, message, strlen(message));
          read_size = recv(sock, client_message, 2000, 0);
          int cash = atoi(client_message);
          balance2 += cash;
          message = "Please, insert cash to the ATM...\nDone!\n";
          write(sock, message, strlen(message));
          message = "------------------------------------------------------\n";
          write(sock, message, strlen(message));
        }
      }

      message = "Enter 1 to go back to menu, 0 to exit: ";
      write(sock, message, strlen(message));
      read_size = recv(sock, client_message, 2000, 0);
      if (client_message[0]=='1') {
          goto MENU;
      }else
          goto EXIT;

  WITHDRAW:
      message = "------------------------------------------------------\n";
      write(sock, message, strlen(message));
      message = "Please, enter your card number: ";
      write(sock, message, strlen(message));
      read_size = recv(sock, client_message, 2000, 0);

      isEqual = 1;
      for (size_t i = 0; i < strlen(card_number1); i++) {
          if (card_number1[i]!=client_message[i]) {
              isEqual = 0;
              break;
            }
      }

      if(isEqual==0){
          isEqual = 2;
          for (size_t i = 0; i < strlen(card_number2); i++) {
              if (card_number2[i]!=client_message[i]) {
                  isEqual = 0;
                  break;
              }
          }
      }

      if ( isEqual == 0 ) {
          message = "No such card number in our bank\n";
          write(sock, message, strlen(message));
      }else{
          if (isEqual==1) {
              message = "Please, enter the amount of money to withdraw: ";
              write(sock, message, strlen(message));
              read_size = recv(sock, client_message, 2000, 0);
              int cash = atoi(client_message);
              if (cash<balance1) {
                   balance1 -= cash;
                   message = "Please, count cash before leaving the ATM...\nDone!\n";
                   write(sock, message, strlen(message));
              }else{
                   message = "Sorry, you don't have enough money.\nYou may want to check your balance.\n";
                   write(sock, message, strlen(message));
              }

              message = "------------------------------------------------------\n";
              write(sock, message, strlen(message));
          }else
          if (isEqual==2) {
              message = "Please, enter the amount of money to withdraw: ";
              write(sock, message, strlen(message));
              read_size = recv(sock, client_message, 2000, 0);
              int cash = atoi(client_message);
              if (cash<balance2) {
                  balance2 -= cash;
                  message = "Please, count cash before leaving the ATM...\nDone!\n";
                  write(sock, message, strlen(message));
              }else{
                  message = "Sorry, you don't have enough money.\nYou may want to check your balance.\n";
                  write(sock, message, strlen(message));
              }

              message = "------------------------------------------------------\n";
              write(sock, message, strlen(message));
            }
      }

      message = "Enter 1 to go back to menu, 0 to exit: ";
      write(sock, message, strlen(message));
      read_size = recv(sock, client_message, 2000, 0);
      if (client_message[0]=='1') {
          goto MENU;
      }else
          goto EXIT;

  TRANSFER:
      message = "------------------------------------------------------\n";
      write(sock, message, strlen(message));
      message = "Please, enter your card number: ";
      write(sock, message, strlen(message));
      read_size = recv(sock, client_message, 2000, 0);
      message = "Please, enter the transaction card number: ";
      write(sock, message, strlen(message));
      read_size = recv(sock, client_message, 2000, 0);
      message = "Please, enter the amount of money to transfer: ";
      write(sock, message, strlen(message));
      read_size = recv(sock, client_message, 2000, 0);
      message = "Please, take the printed payment info before the leaving the ATM.\n";
      write(sock, message, strlen(message));
      message = "------------------------------------------------------\n";
      write(sock, message, strlen(message));

      message = "Enter 1 to go back to menu, 0 to exit: ";
      write(sock, message, strlen(message));
      read_size = recv(sock, client_message, 2000, 0);
      if (client_message[0]=='1') {
          goto MENU;
      }else
          goto EXIT;


  ERROR:
      message = "Thank you for using the ATM, have a nice day!\n";
      write(sock, message, strlen(message));
      if(read_size == 0){
          puts("Client disconnected");
          fflush(stdout);
      }
      else if(read_size == -1){
          perror("recv failed");
      }

  EXIT:
      //shutdown the socket pointer
      while (shutdown(sock, 2)!=0 || close(sock)) {
          perror("Error while closing the socket: ");
      }
      puts("Client disconnected");
      //free up the socket
      free(socket_desc);
      return 0;
  }
