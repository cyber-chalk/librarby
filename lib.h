#ifndef LIBRARY_H
#define LIBRARY_H

// Define the book struct
typedef struct Book {
    struct Book *next;
    int popularity;
    char title[100];
    char author[100];
    int year;
    int returnD;
    int available; // either 1 or 0, can't use bool
} book;

typedef enum sorts {
    SORT_BY_POPULARITY,
    SORT_BY_TITLE,
    SORT_BY_AUTHOR,
    SORT_BY_YEAR,
    SORT_BY_RETURN_DATE,
    SORT_BY_AVAILABILITY
} SortBy;

//inverted index function
typedef struct InvertedIndex {
    char keyword[100];
    book *books;
    struct InvertedIndex *next;
} InvertedIndex;

int compareBooks(const book *a, const book *b, SortBy criteria);
book *readCSV(const char *filename);
int linkSize(book *head);
void divide(book *head, book **start, book **end);
book *sortedMerge(book *a, book *b, SortBy criteria);
book *mergeSort(book *head, SortBy criteria);
void printList(book *head);
book *copyList(book *head);
InvertedIndex *createInvertedIndex(book *head);
book *searchInvertedIndex(InvertedIndex *index, const char *keyword);

#endif //LIBRARY_H

