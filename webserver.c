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

struct Route {
  char *key;
  char *value;

  struct Route *left, *right;
};

struct Route *initRoute(char *key, char *value) {
  struct Route *temp = (struct Route *)malloc(sizeof(struct Route));

  temp->key = key;
  temp->value = value;

  temp->left = temp->right = NULL;
  return temp;
}

void inorder(struct Route *root) {

  if (root != NULL) {
    inorder(root->left);
    printf("%s -> %s \n", root->key, root->value);
    inorder(root->right);
  }
}

struct Route *addRoute(struct Route *root, char *key, char *value) {
  if (root == NULL) {
    return initRoute(key, value);
  }

  if (strcmp(key, root->key) == 0) {
    printf("Warning!!!\nRoute for %s already exists", key);
  } else if (strcmp(key, root->key) > 0) {
    root->right = addRoute(root->right, key, value);
  } else {
    root->left = addRoute(root->left, key, value);
  }
  return root;
}

struct Route *search(struct Route *root, char *key) {
  if (root == NULL) {
    return NULL;
  }

  if (strcmp(key, root->key) == 0) {
    return root;
  } else if (strcmp(key, root->key) > 0) {
    return search(root->right, key);
  } else if (strcmp(key, root->key) < 0) {
    return search(root->left, key);
  }
  printf("not returning anything");
}

void freeRoutes(struct Route *root) {
  if (root != NULL) {
    freeRoutes(root->left);
    freeRoutes(root->right);
    free(root->key);
    free(root->value);
    free(root);
  }
}

char *mimes(char *ext) {

  // clang-format off
    if (strcmp(ext, "html") == 0 || strcmp(ext, "htm") == 0) { return "text/html"; }
    if (strcmp(ext, "jpeg") == 0 || strcmp(ext, "jpg") == 0) { return "image/jpg"; }
    if (strcmp(ext, "css") == 0) { return "text/css"; }
    if (strcmp(ext, "js") == 0) { return "application/javascript"; }
    if (strcmp(ext, "json") == 0) { return "application/json"; }
    if (strcmp(ext, "txt") == 0) { return "text/plain"; }
    if (strcmp(ext, "gif") == 0) { return "image/gif"; }
    if (strcmp(ext, "png") == 0) { return "image/png"; }
  printf("\n missed mimes"); 
  return "text/plain";
  // clang-format on
}

char *headerBuilder(char *ext, int b404, char *header, int size) {
  if (b404 == 2) {
    snprintf(header, size,
             "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n");
  } else {
    snprintf(header, size, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\n\r\n",
             mimes(ext));
  }
  return header;
}

int main() {
  // char header[64] = "HTTP/1.1 200 OK\r\n\n";
  char header404[128] = "HTTP/1.1 404 Not Found\r\n\r\n";
  //  strcat(header, responseData);

  // create a socket
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  // adds routes (hard coded)
  // key value
  struct Route *route = initRoute("/", "./public/index.html");
  addRoute(route, "/styles.css", "./public/styles.css");

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
    printf("\n%s", buffer);

    char *clientHeader = strtok(buffer, "\n"); // automatically adds 0/
    char *method = strtok(clientHeader, " ");
    char *urlRoute = strtok(NULL, " ");
    char *fileType = NULL;
    char *dot = strrchr(urlRoute, '.');
    if (dot && *(dot + 1) != '\0') {
      fileType = dot + 1;
    }
    if (strcmp(urlRoute, "/") == 0)
      fileType = "html";
    printf("\nMethod + Route + Type:%s.%s.%s.\n", method, urlRoute, fileType);

    if (strcmp(method, "GET") == 0) {
      int notFound = 0;
      struct Route *dest = search(route, urlRoute);
      if (dest == NULL)
        notFound = 1;

      char *filePath;
      if (notFound == 1 && fileType != NULL && strcmp(fileType, "html") == 0) {
        filePath = "./public/404.html";
        printf("======404=======");
        notFound = 2;
      }
      if (notFound == 1) {
        send(client, header404, strlen(header404), 0);
        printf("\n-----404!-----\n");
        close(client);
        continue;
      }
      if (notFound != 2) {
        filePath = dest->value; // sometimes this is NULL for some reason
      }
      printf("filePath: %s\n", filePath);

      FILE *fp = fopen(filePath, "r");
      fseek(fp, 0L, SEEK_END);
      size_t size = ftell(fp);
      fclose(fp);
      // printf(" size:%d ", (int)size);

      char template[128];
      char *header = headerBuilder(fileType, notFound, template, 128);
      send(client, header, strlen(header), 0);
      int opened_fd = open(filePath, O_RDONLY);
      sendfile(client, opened_fd, 0, size);

      close(opened_fd);
      close(client);
    } else if (strcmp(method, "POST") == 0) {
    }
  }
  freeRoutes(route);
  close(server_socket);
  return 0;
}
