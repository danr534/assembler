#include "labels_list.h"

labelNode *create_label(char *name, int value, int type) {
    labelNode *newNode = (labelNode *)malloc(sizeof(labelNode));

    newNode->name = name;
    newNode->value = value;
    newNode->type = type;
    newNode->next = NULL;

    return newNode;
}

void append_label(labelNode **head, char *name, int value, int type) {
    labelNode *newNode = create_label(name, value, type);
    labelNode *temp;
    
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

labelNode *search_label(labelNode *head, char *name) {
    labelNode *temp = head;
    while (temp != NULL) {
        if (!strncmp(temp->name, name, strlen(temp->name))) {
            if(*(name + strlen(temp->name)) == ' ' || *(name + strlen(temp->name)) == '\t' || *(name + strlen(temp->name)) == '\n' || *(name + strlen(temp->name)) == '\0') return temp;
        }
        temp = temp->next;
    }
    return NULL;
}

void free_labels_list(labelNode *head) {
    labelNode *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->name);
        free(temp);
    }
}


