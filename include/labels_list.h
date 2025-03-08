#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Label linked list structure */
typedef struct labelNode {
    char *name; /* name of the label */
    int value; /* address of the label in the buffer */
    int type; /* label type (code, data, external...)*/
    struct labelNode *next;
} labelNode;

/** 
 * Creates a new label
 * @param name name of the label
 * @param value address of the label in the buffer
 * @param type label type (code, data, external...)
 * @return pointer to the new label
 */
labelNode *create_label(char *name, int value, int type);

/**
 * Appends a label to the end of the list 
 * @param head pointer of a pointer of a label
 * @param name the labels's name
 * @param value address of the label in the buffer 
 * @param type label type (code, data, external...)
 */
void append_label(labelNode **head, char *name, int value, int type);

/** 
 * Searches a label with a given name in a labels list 
 * @param head pointer to the labels list
 * @param name the name of the label to find
 * @return a pointer to the label with the given name if found, otherwise return NULL
 */
labelNode *search_label(labelNode *head, char *name);

/** 
 * Free the labels list 
 * @param head pointer to the labels list 
 */
void free_labels_list(labelNode *head);

