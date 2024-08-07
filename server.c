#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  char header[2048] = "HTTP/1.1 200 OK\r\n\n";
  //  strcat(header, responseData);

  // create a socket
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  // define the address
  // struct sockaddr_in addr = {AF_INET, htons(8001), INADDR_ANY};
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8001);
  addr.sin_addr.s_addr = INADDR_ANY;
  int bound = bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));

  listen(server_socket, 5);

  while (1) {
    int client = accept(server_socket, NULL, NULL);

    char buffer[256] = {0};
    recv(client, buffer, 256, 0);
    // printf("\n%s", buffer);

    // GET /file.html .....
    send(client, header, strlen(header), 0);
    int opened_fd = open("./public/index.html", O_RDONLY);
    sendfile(client, opened_fd, 0, 256);

    close(opened_fd);
    close(client);
    close(server_socket);
  }

  return 0;
}
