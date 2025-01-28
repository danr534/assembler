#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <errno.h>
#include <String.h>
#include <stdlib.h>

extern char *inputExt;
extern char *extendendInputExt;
extern char *readMode;
extern char *writeMode;

/**
 * Concatinates 2 strings and returns the result without changing the original strings
 * @param str1 the first string
 * @param str2 the second string
 * @return the concatinated string
 */
char *combine_strings(char *str1, char *str2);

/**
 * Opens a given file name
 * @param filename the file name
 * @param extension the file extension
 * @param mode the opening mode
 * @return the file pointer
 */
FILE *open_file(char *filename, char *extension, char *mode);

/**
 * Allocates the content of a file to a buffer
 * @param file the file pointer
 * @param filename the file name
 * @param extension the file extension
 * @param fileSize the file size pointer
 * @return the content buffer
 */
char *get_file_content(FILE *file, char *filename, char *extension, long *fileSize);

/**
 * Removes a file from the project
 * @param filename the name of the file
 * @param extension the extension of the file
 */
void remove_file(char *filename, char *extension);

#endif
