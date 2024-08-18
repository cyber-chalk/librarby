#include "./webserver.h"
#include "./routes.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
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

  temp->key = strdup(key);
  temp->value = strdup(value);

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
    if (strcmp(ext, "ico") == 0) { return "image/png"; }
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

// char *loadFileToString(const char *filePath) {
//   FILE *file = fopen(filePath, "rb");
//   if (!file)
//     return NULL;

//   fseek(file, 0L, SEEK_END);
//   size_t size = ftell(file);
//   fseek(file, 0L, SEEK_SET);

//   char *buffer = malloc(size + 1);
//   if (buffer) {
//     size_t bytesRead = fread(buffer, 1, size, file);
//     if (bytesRead != size) {
//       free(buffer);
//       fclose(file);
//       return NULL;
//     }
//     buffer[size] = '\0'; // Null-terminate the string
//   }

//   fclose(file);
//   return buffer;
// }

int main() {
  // char header[64] = "HTTP/1.1 200 OK\r\n\n";
  // char header404[128] = "HTTP/1.1 404 Not Found\r\n\r\n";
  const char *errorPage = "<html><body>404 Not Found 2</body></html>";
  char header404[256];
  snprintf(header404, sizeof(header404),
           "HTTP/1.1 404 Not Found\r\n"
           "Content-Type: text/html\r\n"
           "Content-Length: %zu\r\n" // Length of the errorPage content
           "\r\n",
           strlen(errorPage));
  //  strcat(header, responseData);

  // create a socket
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);

  // adds routes (hard coded)
  // key value
  struct Route *route = initRoute("/", "./public/index.html");
  addRoute(route, "/styles.css", "./public/styles.css");
  addAllRoutes(route);

  // define the address
  // struct sockaddr_in addr = {AF_INET, htons(8001), INADDR_ANY};
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(8001);
  addr.sin_addr.s_addr = INADDR_ANY;
  bind(server_socket, (struct sockaddr *)&addr, sizeof(addr));

  listen(server_socket, 15);

  while (1) {
    int client = accept(server_socket, NULL, NULL);

    char buffer[512] = {0};
    recv(client, buffer, 256, 0);
    // printf("\n%s", buffer);

    // GET /idk.html?search_queary=popularity HTTP/1.1
    char *clientHeader = strtok(buffer, "\n"); // automatically adds 0/

    char *questionMark = strchr(clientHeader, '?');
    char *method = strtok(clientHeader, " ");
    char *urlRoute = strtok(NULL, questionMark ? "?" : " ");
    char *fileType = NULL;
    char *dot = strrchr(urlRoute, '.');
    if (dot && *(dot + 1) != '\0') {
      fileType = dot + 1;
    }

    char *query = NULL;
    if (strtok(NULL, "="))
      query = strtok(NULL, " ");
    // get popularity, whats after the = and before its next space

    if (strcmp(urlRoute, "/") == 0)
      fileType = "html";
    printf("\nMethod + Route + Type + Query:%s.%s.%s.%s.\n", method, urlRoute,
           fileType, query);

    if (strcmp(method, "GET") == 0) {
      int notFound = 0;

      if (query) {
        // get data.csv sorted accordingly
      }
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
        send(client, errorPage, strlen(errorPage), 0);
        printf("\n-----404!-----\n");
        close(client);
        continue;
      }
      if (notFound != 2) {
        filePath = dest->value;
      }
      printf("filePath: %s\n", filePath);

      FILE *fp = fopen(filePath, "r");
      fseek(fp, 0L, SEEK_END);
      size_t size = ftell(fp);
      fclose(fp);
      // printf(" size:%d ", (int)size);

      char template[128];
      char *header = headerBuilder(fileType, notFound, template, 128);
      int on = 1;
      int off = 0;
      off_t offset = 0;
      ssize_t sent_bytes = 0;
      // setsockopt(client, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
      setsockopt(client, IPPROTO_TCP, TCP_CORK, &on, sizeof(on));
      int opened_fd = open(filePath, O_RDONLY);

      // printf("header Size %ld\nFile size, string %ld %d\n", strlen(header),
      //        size, get_file_size(opened_fd));

      send(client, header, strlen(header), 0);
      // send(client, loadFileToString(filePath), size, 0);
      while (offset < size) {
        ssize_t current_bytes =
            sendfile(client, opened_fd, &offset, size - offset);
        if (current_bytes <= 0) {
          perror("sendfile error");
          exit(EXIT_FAILURE);
          break;
        }
        sent_bytes += current_bytes;
        //       offset -= sent_bytes;
      }
      printf("Offset Sent, Size: %ld, %ld, %ld\n", offset, sent_bytes, size);
      if (sent_bytes < size) {
        fprintf(stderr, "Error: Only %ld of %ld bytes sent\n", sent_bytes,
                size);
        perror("Incomplete sendfile transmission");
      }
      setsockopt(client, IPPROTO_TCP, TCP_CORK, &off, sizeof(off));
      // setsockopt(client, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
      //   sendfile(client, opened_fd, 0, size);
      close(opened_fd);
      close(client);
    } else if (strcmp(method, "POST") == 0) {
      // write to csv
      // if its a search, just return a string through websockers
    }
  }
  freeRoutes(route);
  close(server_socket);
  return 0;
}
