#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <errno.h>
#include <String.h>
#include <stdlib.h>

extern char *inputExt; /* file extension for input files */
extern char *extendedInputExt; /* file extension for extended input files */
extern char *readMode; /* flag to open file in read mode */
extern char *writeMode; /* flag to topen file in write mode */

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
 * @param inputFile the file pointer
 * @param filename the file name
 * @param extension the file extension
 * @param fileSize the file size pointer
 * @return the content buffer
 */
char *get_file_content(FILE *inputFile, char *filename, char *extension, long *fileSize);

/**
 * Removes a file from the project
 * @param filename the name of the file
 * @param extension the extension of the file
 */
void remove_file(char *filename, char *extension);

#endif
