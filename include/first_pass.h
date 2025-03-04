#include "pre_assembler.h"
#include "utils.h"
#include "globals.h"
#include <stdint.h>
#include "ctype.h"

#define INITIAL_CODE_ADDRESS 100
#define WORD_SIZE 24
#define MAX_LABEL_LENGTH 31
#define NUM_REGISTERS 8
#define INT21_MAX (1048575)   /*  2^20 - 1 */
#define INT21_MIN (-1048576)  /* -2^20 */
#define INT24_MAX (16777215) /* 2^24 - 1 */
#define INT24_MIN (-16777216) /* -2^24 */
#define RELATIVE_IC(IC) (IC - INITIAL_CODE_ADDRESS)

/* valid operations names */
enum OPERATIONS {mov, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, jsr, red, prn, rts, stop};

/* valid directives names */
enum DIRECTIVES {data, string, entry, extrn};

/* valid addressing methods names */
enum ADDRESSING_METHODS {immediate, direct, relative, direct_register};

extern int opCodes[NUM_OPERATIONS]; /* valid opcodes with respect to the operations names */
extern int functs[NUM_OPERATIONS]; /* valud functs with respect to the operations names */
extern char registers[NUM_REGISTERS][3]; /* valid registers names */
extern char codeImage[IMAGE_SIZE][7]; /* buffer for the code encoding */
extern char dataImage[IMAGE_SIZE][7]; /* buffer for the data encoding */

/**
 * Scans the current expanded assembly file for the first time.
 * encodes every operation and instruction, save all the labels and encodes all the information words that don't depend on labels locations.
 * @param extendedInputFile file pointer to the current expended assembly file.
 * @param inputName the name of the current input file (without extension) before expanding macros.
 */
void first_pass(FILE *extendedInputFile, char *inputName);