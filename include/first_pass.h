#include "pre_assembler.h"
#include "utils.h"
#include "globals.h"
#include "labels_list.h"
#include <stdint.h>
#include "ctype.h"



extern int opCodes[NUM_OPERATIONS]; /* valid opcodes with respect to the operations names */
extern int functs[NUM_OPERATIONS]; /* valud functs with respect to the operations names */
extern char registers[NUM_REGISTERS][3]; /* valid registers names */
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
 */
void first_pass(FILE *extendedInputFile, char *inputName);