#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Macro linked list structure */
typedef struct macroNode {
    char *name;
    char *content;
    int startIndex;
    int totalLines;
    struct macroNode *next;
} macroNode;

/** 
 * Creates a new macro
 * @param name the macro's name
 * @param content the macro's content 
 * @param startIndex the index of the start line of the macro 
 * @param totalLines the total lines of the macro's content 
 * @return pointer to the new macro
 */
macroNode *create_macro(char *name, char *content, int startIndex, int totalLines);

/**
 * Appends a macro to the end of the list 
 * @param head pointer of a pointer of a macro
 * @param name the macro's name
 * @param content the macro's content 
 * @param startIndex the index of the start line of the macro 
 * @param totalLines the total lines of the macro's content 
 */
void append_macro(macroNode **head, char *name, char *content, int startIndex, int totalLines);

/** 
 * Searches a macro with a given name in a macros list 
 * @param head pointer to the macros list
 * @param name the name of the macro to find
 * @return a pointer to the macro with the given name if found, otherwise return NULL
 */
macroNode *search_macro(macroNode *head, char *name);

/** 
 * Free the macros list 
 * @param head pointer to the macros list 
 */
void free_macros(macroNode *head);

