#include "pre_assembler.h"
#include "utils.h"
#include "globals.h"
#include "labels_list.h"
#include <stdint.h>
#include "ctype.h"

extern const int opCodes[NUM_OPERATIONS]; /* valid opcodes with respect to the operations names */
extern const int functs[NUM_OPERATIONS]; /* valud functs with respect to the operations names */
extern const char registers[NUM_REGISTERS][3]; /* valid registers names */
extern char codeImage[IMAGE_SIZE][7]; /* buffer for the code encoding */
extern char dataImage[IMAGE_SIZE][7]; /* buffer for the data encoding */
extern labelNode *labels; /* list of the labels found until now */
extern int ICF; /* final instruction count value */
extern int DCF; /* final data count value */

/**
 * Scans the current expanded assembly file for the first time.
 * encodes every operation and instruction, save all the labels and encodes all the information words that don't depend on labels locations.
 * @param extendedInputFile file pointer to the current expended assembly file.
 * @param inputName the name of the current input file (without extension) before expanding macros.
 * @param outputName the name of the current file (without extension).
 */
void first_pass(FILE *extendedInputFile, char *inputName, char *outputName);

/** 
 * Adds information word the code encoding buffer.
 * @param ICPtr pointer to IC.
 * @param content the word content.
 * @param A flag that determines if the encoding is absolute (always 1 for operation words).
 * @param R flag that determines if the encoding is relocatable (always 0 for operation words).
 * @param E flag that determines if the encoding is external (always 0 for operation words).
 */
void add_information_word(int *ICPtr, long content, int A, int R, int E);

/**
 * Checks if the operand is a register.
 * @param cursorPtr pointer to the pointer to the operand in the line.
 * @return the number of the register (-1 if the operand is not a register).
 */
int is_reg_operand(char **cursorPtr);