#ifndef LIBRARY_H
#define LIBRARY_H


// Define the book struct
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
    SORT_BY_RETURN_DATE
} SortBy;

// Function prototypes
int compareBooks(const book *a, const book *b, SortBy criteria);
book *readCSV(const char *filename);
void printList(book *head);
book *copyList(book *head);
void LOWERCASE(char *text, char *result);

void viewReturnDate(book *head, const char *name);
void issueDate(book *head, const char *name, const char *issueD);
void clearReturnDate( book *head, const char *name);

void deleteBook(book **headRef, char delTitle[], SortBy criteria)
void addBook(book **headRef, int newPopularity, const char *newTitle, const char *newAuthor, int newYear, int newReturnD, int newAvailable, SortBy criteria);

typedef struct searchList_ {
    book *thisBook;
    int distance;
    struct searchList_ *next;
} searchList;

searchList *searchHelper(char *const input, book *head);
int listToArray(searchList *head, searchList ***array);
int compareByDist(const void *a, const void *b);

#endif // LIBRARY_H

