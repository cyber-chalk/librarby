#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
// Use a binary search if the user is searching by the thing the data is already
// sorted by (ie. popularity) otherwise, just use a linear search or some string
// search This way you will be using the data on the heap I know this isn't
// really clear so feel free to dm me or post a q&a on discussions

// Popularity,Title,Author,Year,Return,Available
typedef struct Book {
  struct Book *next;
  int popularity;
  char title[100];
  char author[100];
  int year;
  int returnD;
  int available; // either 1 or 0, cant use bool
} book;

book *readCSV(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("file not opening");
    return NULL;
  }
  char line[256];
  book *current = NULL;
  book *head;

  while (fgets(line, sizeof(line), file)) {
    book *newBook = malloc(sizeof(book));

    // - %d: Read an integer (popularity, year, returnD, available)
    // - %99[^,]: Read a string (up to 99 characters or until a comma) (title,
    // author)
    sscanf(line, "%d,%99[^,],%99[^,],%d,%d,%d", &newBook->popularity,
           newBook->title, newBook->author, &newBook->year, &newBook->returnD,
           &newBook->available);
    newBook->next = NULL;

    if (current == NULL) {
      current = newBook;
      head = newBook;
    } else {
      current->next = newBook;
      current = newBook;
    }
  }
  fclose(file);
  return head;
}

int main() { return 0; }
