#ifndef GLOBALS_H
#define GLOBALS_H

#define INPUT_DIR "input/" /* input dir */
#define OUTPUT_DIR "output/" /* output dir */
#define MAX_LINE_LENGTH 82 /* maximum length of every line */
#define IMAGE_SIZE 200 /* size of the code and data buffers */
#define NUM_OPERATIONS 16 /* number of operations */
#define NUM_DIRECTIVES 4 /* number of directives */
#define INITIAL_CODE_ADDRESS 100 /* initial code buffer address */
#define WORD_SIZE 24 /* word size */
#define MAX_LABEL_LENGTH 31 /* maximum label length */
#define NUM_REGISTERS 8 /* number of registers */
#define INT21_MAX (1048575)   /*  2^20 - 1 */
#define INT21_MIN (-1048576)  /* -2^20 */
#define INT24_MAX (8388607) /* 2^23 - 1 */
#define INT24_MIN (-8388608) /* -2^23 */
#define RELATIVE_IC(IC) (IC - INITIAL_CODE_ADDRESS) /* relative code address in respect to the initial code address */

/* valid operations names */
enum OPERATIONS {mov, cmp, add, sub, lea, clr, not, inc, dec, jmp, bne, jsr, red, prn, rts, stop};

/* valid directives names */
enum DIRECTIVES {data, string, entry, extrn};

/* valid addressing methods names */
enum ADDRESSING_METHODS {immediate, direct, relative, direct_register};

/* label types */
enum LABEL_TYPES {_code, _data, _code_entry, _data_entry, _external, _unidentified_entry};


#endif