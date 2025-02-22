#include "macros_list.h"

macroNode *create_macro(char *name, char *content, int startIndex, int totalLines) {
    macroNode *newNode = (macroNode *)malloc(sizeof(macroNode));

    newNode->name = name;
    newNode->content = content;
    newNode->startIndex = startIndex;
    newNode->totalLines = totalLines;
    newNode->next = NULL;

    return newNode;
}

void append_macro(macroNode **head, char *name, char *content, int startIndex, int totalLines) {
    macroNode *newNode = create_macro(name, content, startIndex, totalLines);
    macroNode *temp;
    
    if (*head == NULL) {
        *head = newNode;
        return;
    }

    temp = *head;
    while (temp->next != NULL) {
        temp = temp->next;
    }
    temp->next = newNode;
}

macroNode *search_macro(macroNode *head, char *name) {
    macroNode *temp = head;
    while (temp != NULL) {
        if (!strncmp(temp->name, name, strlen(temp->name))) {
            if(*(name + strlen(temp->name)) == ' ' || *(name + strlen(temp->name)) == '\t' || *(name + strlen(temp->name)) == '\n' || *(name + strlen(temp->name)) == '\0') return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void free_macros(macroNode *head) {
    macroNode *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->name);
        free(temp->content);
        free(temp);
    }
}


