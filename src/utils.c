#include "utils.h"

char *inputExt = ".as";
char *extendendInputExt = ".am";
char *readMode = "r";
char *writeMode = "w";

char *combine_strings(char *str1, char *str2) {
    int length = strlen(str1) + strlen(str2) + 1;
    char *result = (char *)malloc(length * sizeof(char));
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

FILE *open_file(char *filename, char *extension, char *mode) {

    char *fullName = combine_strings(filename, extension); /* the filename with .as extension */
    FILE *file; /* the file pointer */

    /* open the file */
    file = fopen(fullName, mode);
    if (file == NULL) {
        if (!strcmp(mode, readMode)) printf("Error opening file '%s': ", fullName);
        else printf("Error creating file '%s': ", fullName);
        perror("");
    }
    /* free memory for fullName variable */
    free(fullName);
    return file;
}

char *get_file_content(FILE *file, char *filename, char *extension, long *fileSize) {
    char *buffer; /* the buffer to save the file content */
    size_t bytesRead; /* the number of successfull bytes copied from the file */

     /* move to the end of the file to determine its size */
    fseek(file, 0, SEEK_END);
    *fileSize = ftell(file);
    rewind(file);

    /* allocate memory for the string (+1 for the null terminator) */
    buffer = (char *)malloc((*fileSize + 1) * sizeof(char));
    if (buffer == NULL) {
        printf("Failed to allocate memory for file '%s%s': ", filename, extension);
        perror("");
        fclose(file);
        return NULL;
    }

    /* Read the file contents into the buffer */
    bytesRead = fread(buffer, sizeof(char), *fileSize, file);
    buffer[bytesRead] = '\0';

    /* close file */
    fclose(file);

    /* return the buffer */
    return buffer;
}

void remove_file(char *filename, char *extension) {
    char *fullName = combine_strings(filename, extension);
    int removeError = remove(fullName);
    if(removeError) {
        printf("Error deleting file '%s': ", fullName);
        perror("");
    }
    free(fullName);
}