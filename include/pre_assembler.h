#include <stdio.h>
#include <stdlib.h>
#include <String.h>
#include "globals.h"

extern int *translateIndexes; /* for every line number i in the extended input file, translateIndexes[i] stores the corresponding line number in the input file */
extern int numLines; /* the number of lines in the extended input file */
extern int isError; /* an flag for error in the compilation process */
extern const char *instructions[]; /* a list of all the possible instructions */
extern const char *directives[]; /* a list of all the possible directives */

/**
 * Reads input file buffer and substitutes every macro call with its corresponding code block.
 * The result is stored in an extended input file with the extension '.am'.
 * @param buffer the input file buffer.
 * @param inputFileSize the input file size in bytes.
 * @param filename the name of the output file (without extension).
*/
void expand_macros(char *buffer, long inputFileSize, char *filename);