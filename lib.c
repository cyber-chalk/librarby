#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #define _XOPEN_SOURCE
#define _GNU_SOURCE
#define __USE_XOPEN
#include <time.h> 
// merge sort by every way to sort and have that in the heap, so when the user
// requests it, we will have it ready. When a addition/deletion is made,
// we will update it in every list in the heap and the csv as well.
// Concerning searching, we will use a inverted index which is best for
// keywords.

// Popularity,Title,Author,Year,Return,
typedef struct Book {
  struct Book *next;
  int popularity;
  char title[100];
  char author[100];
  int year;
  char returnD[100];  
} book;

typedef enum sorts {
  SORT_BY_POPULARITY,
  SORT_BY_TITLE,
  SORT_BY_AUTHOR,
  SORT_BY_YEAR,
  SORT_BY_RETURN_DATE,
 
} SortBy;

int timeCheck(book* a, book* b) {
  struct tm tm1 = {0}, tm2 = {0};
  time_t t1, t2;

  strptime(a->returnD, "%d-%m-%Y", &tm1);
  strptime(b->returnD, "%d-%m-%Y", &tm2);

  t1 = mktime(&tm1);
  t2 = mktime(&tm2);

  double seconds = difftime(t1, t2);
  return seconds / (60 * 60 * 24);

}

int compareBooks(const book *a, const book *b, SortBy criteria) {
  switch (criteria) {
  case SORT_BY_POPULARITY:
    return a->popularity - b->popularity;
  case SORT_BY_TITLE:
    return strcmp(a->title, b->title);
  case SORT_BY_AUTHOR:
    return strcmp(a->author, b->author);
  case SORT_BY_YEAR:
    return a->year - b->year;
  case SORT_BY_RETURN_DATE:
    return timeCheck(a,  b);
  default:
    return 0;
  }
}

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

    // - %d: Read an integer (popularity,Title, Author, year, returnD)
    // - %99[^,]: Read a string (up to 99 characters or until a comma) (title,
    // author)
    sscanf(line, "%d,%99[^,],%99[^,],%d,%99[^,]", &newBook->popularity,
           newBook->title, newBook->author, &newBook->year, newBook->returnD);
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
// Merge sort is often preferred for sorting a linked list.
// The slow random-access performance of a linked list makes other algorithms
// like Quick Sort perform poorly, and other like Heap Sort completely
// impossible. - Geeks for Geeks

int linkSize(book *head) {
  int size = 0;
  while (head->next != NULL) {
    size++;
    head = head->next;
  }
  return size;
}

void divide(book *head, book **start, book **end) {
  *start = head;
  book *current = head->next;
  book *prev = head;

  while (current != NULL) {
    current = current->next;
    if (current != NULL) {
      prev = prev->next;
      current = current->next;
    }
  }
  *end = prev->next;
  prev->next = NULL;
}

book *sortedMerge(book *a, book *b, SortBy criteria) {
  book *head; // maybe remove

  if (a == NULL)
    return (b);
  else if (b == NULL)
    return (a);
  if (compareBooks(a, b, criteria) <= 0) {
    head = a;
    head->next = sortedMerge(a->next, b, criteria);
  } else {
    head = b;
    head->next = sortedMerge(a, b->next, criteria);
  }

  return head;
}

book *mergeSort(book *head, SortBy criteria) {
  // int size = linkSize(head);
  book *a;
  book *b;

  if ((head == NULL) || (head->next == NULL))
    return head;
  divide(head, &a, &b);

  mergeSort(a, criteria);
  mergeSort(b, criteria);

  return head = sortedMerge(a, b, criteria);
}

void printList(book *head) {
  book *current = head;
  while (current != NULL) {
    printf("Popularity: %d\n", current->popularity);
    printf("--------------------------\n");
    current = current->next;
  }
}

book *copyList(book *head) {
  if (head == NULL)
    return NULL;

  // Allocate memory for the new book node
  book *newBook = malloc(sizeof(book));
  if (newBook == NULL) {
    printf("Memory allocation failed\n");
    return NULL;
  }

  // Copy the data from the original node to the new node
  newBook->popularity = head->popularity;
  strcpy(newBook->title, head->title);
  strcpy(newBook->author, head->author);
  newBook->year = head->year;
  strcpy(newBook->returnD, head->returnD);
  // newBook->returnD = head->returnD;


  // Recursively copy the rest of the list
  newBook->next = copyList(head->next);

  return newBook;
}

int main() {
  book *popularityH = readCSV("./data.csv");
  book *titleH = mergeSort(copyList(popularityH), SORT_BY_TITLE);
  book *authorH = mergeSort(copyList(popularityH), SORT_BY_AUTHOR);
  book *yearH = mergeSort(copyList(popularityH), SORT_BY_YEAR);
  book *returnH = mergeSort(copyList(popularityH), SORT_BY_RETURN_DATE);

  //  on front-end disconnection make sure to free
  //  Also! may need to move the code above this to main.c
  printList(returnH);
  // printList(readCSV("./data.csv"));

  return 0;
}
