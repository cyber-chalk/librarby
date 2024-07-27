#include <stdio.h>
#include <stdbool.h>
// Use a binary search if the user is searching by the thing the data is already sorted by (ie. popularity) 
// otherwise, just use a linear search or some string search
// This way you will be using the data on the heap
// I know this isn't really clear so feel free to dm me or post a q&a on discussions

// Popularity,Title,Author,Year,Return,Available
typedef struct Book {
  struct Book* next;
  int popularity;
  char title[100];
  char author[100];
  int returnD;
  bool available;
} book;

int main() {
  
  return 1;
}
