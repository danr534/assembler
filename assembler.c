#include "pre_assembler.h"

/* Gets a filename, adds the .as extension, opens it and returns the file pointer */
FILE *open_file(char *filename);

int main(int argc, char *argv[]) {
    int currentIndex; /* the index of the current assembly file */
    FILE *currentFile; /* the pointer of the current assembly file */
    char *filename; /* the name of the current assembly file */

    /* check that there is at least one command line argument */
    if(argc < 2) {
        printf("Missing command line argument, please enter at least one assembly file.\n");
        exit(1);
    }

    /* iterate over each assembly file and compile it */
    for(currentIndex = 1; currentIndex < argc; currentIndex++) {
        filename = argv[currentIndex];
        currentFile = open_file(filename);
        expand_macros(currentFile, filename);
        
        fclose(currentFile);
    }
    return 0;
}

FILE *open_file(char *filename) {
    int length = strlen(filename) + 4; /* the length of the concatinated filename, .as extension and a null terminator*/
    char *fullFilename = (char *)malloc(length * sizeof(char)); /* the filename with .as extension */
    FILE *currentFile; /* the file pointer */

    /* assign the filename with .as extension to the variable fullFilename */
    strcpy(fullFilename, filename); 
    strcat(fullFilename, ".as");

    /* open the file */
    currentFile = fopen(fullFilename, "r");
    if (currentFile == NULL) {
        printf("Error opening file, please ensure that the file: '%s' exists.\n", fullFilename);
        exit(1);
    }

    /* free memory for fullFilename variable */
    free(fullFilename);

    /* return the file pointer */
    return currentFile;
}