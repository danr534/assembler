#include "first_pass.h"

/**
 * Scans the current expanded assembly file for the second time.
 * encodes every operation and instruction that depend on labels locations and write all the encondings to output files.
 * @param extendedInputFile file pointer to the current expended assembly file.
 * @param inputName the name of the current input file (without extension) before expanding macros.
 * @param outputName the name of the current output file (without extension).
 */
void second_pass(FILE *extendedInputFile, char *inputName, char *outputName);