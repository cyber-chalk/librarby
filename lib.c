#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #define _XOPEN_SOURCE
#define _GNU_SOURCE
#define __USE_XOPEN
#include <ctype.h>
#include <time.h>
#define MIN_DISTANCE 8
#define MIN3(a, b, c)                                                          \
  ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))

// Popularity,Title,Author,Year,Return,
typedef struct Book {
  struct Book *next;
  int popularity;
  char title[100];
  char author[100];
  int year;
  char returnD[100];
} book;

typedef struct searchList_ {
  book *thisBook;
  int distance;
  struct searchList_ *next;
} searchList;

typedef enum sorts {
  SORT_BY_POPULARITY,
  SORT_BY_TITLE,
  SORT_BY_AUTHOR,
  SORT_BY_YEAR,
  SORT_BY_RETURN_DATE,

} SortBy;

int timeCheck(const book *a, const book *b) {
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
    return timeCheck(a, b);
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
  book *head = NULL;

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

int linkSize(book *head) {
  int size = 0;
  while (head != NULL) {
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
  book *result = NULL;

  if (a == NULL)
    return b;
  else if (b == NULL)
    return a;

  if (compareBooks(a, b, criteria) <= 0) {
    result = a;
    result->next = sortedMerge(a->next, b, criteria);
  } else {
    result = b;
    result->next = sortedMerge(a, b->next, criteria);
  }
  return result;
}

book *mergeSort(book *head, SortBy criteria) {
  if (head == NULL || head->next == NULL)
    return head;

  book *a;
  book *b;
  divide(head, &a, &b);

  a = mergeSort(a, criteria);
  b = mergeSort(b, criteria);

  return sortedMerge(a, b, criteria);
}

void printList(book *head) {
  book *current = head;
  while (current != NULL) {
    printf("%d %s %s %d %s n", current->popularity, current->title,
           current->author, current->year, current->returnD);
    current = current->next;
  }
}

book *copyList(book *head) {
  if (head == NULL)
    return NULL;

  book *newBook = malloc(sizeof(book));

  // Copy the data from the original node to the new node
  newBook->popularity = head->popularity;
  strcpy(newBook->title, head->title);
  strcpy(newBook->author, head->author);
  newBook->year = head->year;
  strcpy(newBook->returnD, head->returnD);
  newBook->next = copyList(head->next);
  // newBook->returnD = head->returnD;

  return newBook;
}

const int levenshtein(char *s1, char *s2) {
  unsigned int s1len, s2len, x, y, lastdiag, olddiag;
  s1len = strlen(s1);
  s2len = strlen(s2);
  unsigned int column[s1len + 1];
  for (y = 1; y <= s1len; y++)
    column[y] = y;
  for (x = 1; x <= s2len; x++) {
    column[0] = x;
    for (y = 1, lastdiag = x - 1; y <= s1len; y++) {
      olddiag = column[y];
      column[y] = MIN3(column[y] + 1, column[y - 1] + 1,
                       lastdiag + (s1[y - 1] == s2[x - 1] ? 0 : 1));
      lastdiag = olddiag;
    }
  }
  return column[s1len];
}

void LOWERCASE(char *text, char *result) {
  int length = strlen(text);
  for (size_t i = 0; i < length; i++) {
    result[i] = tolower((unsigned char)text[i]);
  }
  result[length] = '\0';
}

searchList *searchHelper(char *const input, book *head) {
  int length = strlen(input);
  char searchText[length + 1];
  LOWERCASE(input, searchText);
  // Filter
  searchList *filteredHead = NULL; // head of list
  searchList *last = NULL;

  while (head != NULL) {
    int pSize = strlen(head->title);
    char *booklowc = malloc(pSize + 1);
    LOWERCASE(head->title, booklowc);
    const int distance = levenshtein(booklowc, searchText);
    // searchlist
    int pass = 0;
    if (strstr(booklowc, searchText))
      pass = 1;
    if (distance <= MIN_DISTANCE || pass) {
      searchList *filteredBook = malloc(sizeof(searchList));
      filteredBook->thisBook = head;
      filteredBook->distance = pass ? 0 : distance;
      filteredBook->next = NULL;
      if (filteredHead == NULL) {
        filteredHead = filteredBook;
        last = filteredBook;
      } else {
        last->next = filteredBook;
        last = filteredBook;
      }
    }
    head = head->next;
    free(booklowc);
  }
  return filteredHead;
}

// delete later
void printList2(searchList *head) {
  searchList *current = head;
  char *str1 = "gatsby";
  while (current != NULL) {
    char *str2 = current->thisBook->title;
    printf("in list: %s difference: %d\n", current->thisBook->title,
           levenshtein(str2, str1));
    printf("--------------------------\n");
    current = current->next;
  }
}

int listToArray(searchList *head, searchList ***array) {
  int count = 0;
  searchList *current = head;
  while (current != NULL) {
    count++;
    current = current->next;
  }
  *array = (searchList **)malloc(count * sizeof(searchList *));
  current = head;
  for (int i = 0; i < count; i++) {
    (*array)[i] = current;
    current = current->next;
  }
  return count;
}

int compareByDist(const void *a, const void *b) {
  const searchList *sa = *(const searchList **)a;
  const searchList *sb = *(const searchList **)b;
  return sa->distance - sb->distance;
}
// search list to have dist

void freeBooks(book *head) {
  book *current = head;
  while (current != NULL) {
    book *next = current->next;
    free(current);
    current = next;
  }
}

// view the return dates on books
void viewReturnDate(book *head, const char *name) {
  while (head != NULL) {
    if (strcmp(head->title, name) == 0) {
      if (strcmp(head->returnD, "00-00-0001") != 0) {
        printf("the return date for %s is: %s\n", name, head->returnD);
      } else {
        printf("'%s' does not have a return date. use 'issueDate' to issue a "
               "date to this book\n",
               name);
      }
      return;
    }
    head = head->next;
  }
  printf("could not find a book with the title '%s'\n", name);
}

// issue a return date to a book
void issueDate(book *head, const char *name, const char *issueD) {
  while (head != NULL) {
    if (strcmp(head->title, name) == 0) {
      if (strcmp(head->returnD, "00-00-0001") == 0) {
        strcpy(head->returnD, issueD);

        printf("%s has been given the issue date of '%s'\n", name, issueD);
      } else {
        printf("this book already has a return date, '%s'. use "
               "'clearReturnDate' to clear it first\n",
               head->returnD);
      }
      return;
    }
    head = head->next;
  }
  printf("could not find a book with the title '%s'\n", name);
}

// clear previous return dates and set to default
void clearReturnDate(book *head, const char *name) {
  while (head != NULL) {
    if (strcmp(head->title, name) == 0) {
      strcpy(head->returnD, "00-00-0001");
      return;
    }
    head = head->next;
  }
  printf("could not find a book with the title '%s'\n", name);
}

// int main() {
//  book *popularityH = readCSV("./public/data.csv");
//  if (popularityH == NULL) {
//    printf("fail");
//  }
//  // book *titleH = mergeSort(copyList(popularityH), SORT_BY_TITLE);
//  // book *authorH = mergeSort(copyList(popularityH), SORT_BY_AUTHOR);
//  // book *yearH = mergeSort(copyList(popularityH), SORT_BY_YEAR);
//  // book *returnH = mergeSort(copyList(popularityH), SORT_BY_RETURN_DATE);

// // just use qsort to sort because im too lazy to modify the merge sort

// // printList2(searchHelper("Gatsby", popularityH));

// searchList *filteredList = searchHelper("Gatsby", popularityH);
// searchList **array;
// int size = listToArray(filteredList, &array);
// qsort(array, size, sizeof(searchList *), compareByDist);
// for (int i = 0; i < size; i++) {
//   printf("Title: %s, Distance: %d\n", array[i]->thisBook->title,
//          array[i]->distance);
// }

// // Free the allocated memory for the array
// searchList *current = filteredList;
// while (current != NULL) {
//   searchList *temp = current;
//   current = current->next;
//   free(temp);
// }
// free(array);
// freeBooks(popularityH);
// char *str1 = "1984";
// char *str2 = "gatsby";

// int distance = levenshtein(str1, str2);
// printf("\nLevenshtein distance between '%s' and '%s' is %d\n", str1, str2,
//        distance);

// On front-end disconnection make sure to free
// Also! may need to move the code above this to main.c
// printList(yearH);
// printList(readCSV("./data.csv"));

// return 0;
//}
