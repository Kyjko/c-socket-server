#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

#define PORT 4000
#define BUFSIZE 4096
#define MAXPATH 1024

#define SOCKETERR (-1)

void handle_connection(int client_socket);
int check(int exp, const char* msg);

int main(int argc, char** argv) {
  int server_socket, client_socket, addr_size;
  SA_IN server_addr, client_addr;

  check((server_socket = socket(AF_INET, SOCK_STREAM, 0)),
  "Failed to create socket!");

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  check(bind(server_socket, (SA*)&server_addr, sizeof(server_addr)),
  "Couldn't bind socket!");

  check(listen(server_socket, 1),
  "Listen failed!");

  while(1) {
    printf("Awaiting connection...\n");
    addr_size = sizeof(SA_IN);
    check(client_socket =
      accept(server_socket, (SA*)&client_addr, (socklen_t*)&addr_size),
    "Accept failed!");
    printf("Connected!\n");

    handle_connection(client_socket);
  }

  close(server_socket);

  return 0;
}

int check(int exp, const char* msg) {
  if(exp == SOCKETERR) {
    perror(msg);
    exit(-1);
  }

  return exp;
}

void handle_connection(int client_socket) {
  char buffer[BUFSIZE];
  size_t bytes_read;
  int msgsize = 0;
  char actualpath[MAXPATH+1];

  while((bytes_read = read(client_socket, buffer+msgsize, sizeof(buffer)-msgsize-1)) > 0) {
    msgsize += bytes_read;
    printf("reading...\n");
    if(msgsize > BUFSIZE - 1 || buffer[msgsize-1] == '\n') break;
  }

  check(bytes_read, "Receive error!");
  fflush(stdout);

  buffer[strcspn(buffer, "\n")] = 0;

  if(realpath(buffer, actualpath) == NULL) {
    printf("BAD PATH : %s\n", buffer);
    close(client_socket);
    return;
  }

  FILE* fp = fopen(actualpath, "r");
  if(fp == NULL) {
    printf("Couldn't open requested file!");
    return;
  }

  while((bytes_read = fread(buffer, 1, BUFSIZE, fp)) > 0) {
    printf("sending %zu bytes...\n", bytes_read);
    write(client_socket, buffer, bytes_read);
  }

  close(client_socket);
  fclose(fp);
  printf(" - Connection closed - \n");

}
