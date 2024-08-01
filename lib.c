#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "library.h"

// Popularity,Title,Author,Year,Return,Available

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
            return a->returnD - b->returnD;
        case SORT_BY_AVAILABILITY:
            return a->available - b->available;
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
        printf("Title: %s\nAuthor: %s\nPopularity: %d\nYear: %d\nReturn Date: %d\nAvailable: %d\n\n", 
               current->title, current->author, current->popularity, 
               current->year, current->returnD, current->available);
        current = current->next;
    }
}

book *copyList(book *head) {
    if (head == NULL)
        return NULL;

    book *newBook = malloc(sizeof(book));
    if (newBook == NULL) {
        printf("Memory allocation failed\n");
        return NULL;
    }

    newBook->popularity = head->popularity;
    strcpy(newBook->title, head->title);
    strcpy(newBook->author, head->author);
    newBook->year = head->year;
    newBook->returnD = head->returnD;
    newBook->available = head->available;
    newBook->next = copyList(head->next);

    return newBook;
}

InvertedIndex *createInvertedIndex(book *head) {
    InvertedIndex *index = NULL;

    book *current = head;
    while (current != NULL) {
        // Create keywords (for simplicity, consider only title and author)
        char *keywords[] = {current->title, current->author};

        for (int i = 0; i < 2; i++) {
            InvertedIndex *existingNode = index;
            InvertedIndex *prevNode = NULL;

            while (existingNode != NULL && strcmp(existingNode->keyword, keywords[i]) != 0) {
                prevNode = existingNode;
                existingNode = existingNode->next;
            }

            if (existingNode == NULL) {
                InvertedIndex *newNode = malloc(sizeof(InvertedIndex));
                strcpy(newNode->keyword, keywords[i]);
                newNode->books = copyList(current);
                newNode->next = NULL;

                if (prevNode == NULL) {
                    index = newNode;
                } else {
                    prevNode->next = newNode;
                }
            } else {
                book *bookList = existingNode->books;
                while (bookList->next != NULL) {
                    bookList = bookList->next;
                }
                bookList->next = copyList(current);
            }
        }
        current = current->next;
    }
    return index;
}

book *searchInvertedIndex(InvertedIndex *index, const char *keyword) {
    while (index != NULL) {
        if (strcmp(index->keyword, keyword) == 0) {
            return index->books;
        }
        index = index->next;
    }
    return NULL;
}

int main() {
    book *popularityH = readCSV("./data.csv");
    if (popularityH == NULL) {
        return 1;
    }

    // Merge sort by every way to sort and have that in the heap, so when the user
    // requests it, we will have it ready. When an addition/deletion is made,
    // we will update it in every list in the heap and the csv as well.
    // Concerning searching, we will use an inverted index which is best for
    // keywords.

    book *titleH = mergeSort(copyList(popularityH), SORT_BY_TITLE);
    book *authorH = mergeSort(copyList(popularityH), SORT_BY_AUTHOR);
    book *yearH = mergeSort(copyList(popularityH), SORT_BY_YEAR);
    book *returnH = mergeSort(copyList(popularityH), SORT_BY_RETURN_DATE);
    book *availabilityH = mergeSort(copyList(popularityH), SORT_BY_AVAILABILITY);

    // On front-end disconnection make sure to free
    // Also! may need to move the code above this to main.c

    // Create the inverted index
    InvertedIndex *index = createInvertedIndex(popularityH);

    char keyword[100];
    printf("Enter a keyword to search: ");
    scanf("%99s", keyword);

    book *result = searchInvertedIndex(index, keyword);
    if (result != NULL) {
        printList(result);
    } else {
        printf("No results found for keyword: %s\n", keyword);
    }

    return 0;
}
