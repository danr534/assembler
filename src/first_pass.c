#include "first_pass.h"

const int opCodes[] = {0, 1, 2, 2, 4, 5, 5, 5, 5, 9, 9, 9, 12, 13, 14, 15};
const int functs[] = {0, 0, 1, 2, 0, 1, 2, 3, 4, 1, 2, 3, 0, 0, 0, 0};
const char registers[][3] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
char codeImage[IMAGE_SIZE][7];
char dataImage[IMAGE_SIZE][7];
labelNode *labels; 
int ICF;
int DCF;

/**
 * Encodes the operation in the current line.
 * @param fullInputName the full name of the input file.
 * @param line line counter in the input file.
 * @param cursor pointer of the current line (skips blank characters at the beginning).
 * @param op the operation number in respect to the operations names
 * @param ICPtr pointer to IC
 */
void read_operation(char *fullInputName, int line, char *cursor, int op, int *ICPtr);

/** 
 * Adds operation word to the code encoding buffer.
 * @param ICPtr pointer to IC.
 * @param opCode opCode of the operation.
 * @param source_addressing the addressing method of the source operand.
 * @param source_register the register number of the source operand (0 if the source operand is not a register).
 * @param target_addressing the addressing method of the target operand.
 * @param funct funct of the operation.
 * @param A flag that determines if the encoding is absolute (always 1 for operation words).
 * @param R flag that determines if the encoding is relocatable (always 0 for operation words).
 * @param E flag that determines if the encoding is external (always 0 for operation words).
 */
void add_operation_word(int *ICPtr, const int opcode, int source_addressing, int source_register, int target_addressing, int target_register, const int funct, int A, int R, int E);

/**
 * Decodes operation line with a single operand
 * @param cursor pointer to the operand in the line.
 * @param fullInputName the full name of the input file.
 * @param line the line number in the input file.
 * @param operation the operation name.
 * @param ICPtr pointer to IC. 
 */
void decode_single_operand_operation(char *cursor, char *fullInputName, int line, const char *operation, int op, int *ICPtr);

/**
 * Extracts immediate value of an operand.
 * @param cursorPtr pointer to the pointer to the operand in the line.
 * @param fullInputName the full name of the input file.
 * @param line the line number in the input file.
 * @param type the operand type (operation or data).
 * @return the immediate value as integer.
 */
long extract_immediate_value(char **cursorPtr, char *fullInputName, int line, char *type);

/**
 * Checks if the label is valid.
 * @param cursorPtr pointer to the pointer to the label in the line.
 * @param fullInputName the full name of the input file.
 * @param line the line number in the input file.
 * @return 1 if the label is valid, 0 otherwise.
 */
int is_valid_label(char **cursorPtr, char *fullInputName, int line);


/**
 * Decodes operation line with 2 operands.
 * @param cursor pointer to the operand in the line.
 * @param fullInputName the full name of the input file.
 * @param line the line number in the input file.
 * @param operation the operation name.
 * @param ICPtr pointer to IC. 
 */
void decode_two_operands_operation(char *cursor, char *fullInputName, int line, const char *operation, int op, int *ICPtr);

/**
 * Encodes the directive in the current line in the data buffer.
 * @param fullInputName the full name of the input file.
 * @param line line counter in the input file.
 * @param cursor pointer of the current line (skips blank characters at the beginning).
 * @param directive the directive number in respect to the operations names
 * @param DCPtr pointer to DC
 */
void read_directive(char *fullInputName, int line, char *cursor, int directive, int *DCPtr);

/**
 * Reads the label at the beginning of the line and adds it to the label list.
 * @param fullInputName the full name of the input file.
 * @param line line counter in the input file.
 * @param cursor pointer of the current line (skips blank characters at the beginning).
 * @param ICPtr pointer to IC.
 * @param DCPtr pointer to DC.
 */
void read_label(char *fullInputName, int line, char *cursor, int *ICPtr, int *DCPtr);

/**
 * Adds ICF to the data labels values to seperate the data and code segments
 */
void update_data_labels();


void first_pass(FILE *extendedInputFile, char *inputName, char *outputName) {
    char *cursor = (char *)malloc(MAX_LINE_LENGTH * sizeof(char)); /* pointer of the current line */
    int counter = 1; /* line counter in the expanded input file */
    int IC = INITIAL_CODE_ADDRESS; /* counter of the code encoded words */
    int DC = 0; /* counter of the data encoded words */
    char *fullInputName = combine_strings(inputName, inputExt); /* the full name of the input file */
    int is_read; /* a boolean variable that is 1 if the line is already read and 0 otherwise */
    char *tmp; /* temporary pointer to check that the operation is not a prefix of a label */
    int i;
    labels = NULL; /* initialize labels list */
    
    
    /* iterate over each line in the expended input file and encode it */
    while(fgets(cursor, MAX_LINE_LENGTH, extendedInputFile) != NULL) {
        is_read = 0;
        while(*cursor == ' ' || *cursor == '\t') cursor++; /* skip over white characters */

        /* check if the current line is an operation */
        for(i = 0; i < NUM_OPERATIONS; i++) {
            if(!strncmp(cursor, operations[i], strlen(operations[i]))) {
                tmp = cursor + strlen(operations[i]);
                if(*tmp == ' ' || *tmp == '\t' || *tmp == '\n' || *tmp == '\0') {
                    read_operation(fullInputName, translateIndexes[counter], cursor, i, &IC);
                    is_read = 1;
                }
            }
        }

        /* check if the current line is a directive */
        for(i = 0; i < NUM_DIRECTIVES; i++) {
            if(!strncmp(cursor, directives[i], strlen(directives[i]))) {
                read_directive(fullInputName, translateIndexes[counter], cursor, i, &DC);
                is_read = 1;
            }
        }
        
        /* ensure the line isn't empty or a comment */
        if(*cursor != ';' && *cursor != '\n' && *cursor != '\0' && is_read == 0) {
            read_label(fullInputName, translateIndexes[counter], cursor, &IC, &DC);
        }
        counter++;
    }
    
    /* free the dynamic memory */
    free_macros_list(macros);
    free(cursor);
    free(fullInputName);

    /* set ICF and DCF */
    ICF = IC;
    DCF = DC;

    /* add ICF to the data lables values */
    update_data_labels();

    /* handle error */
    if(isError) {
        free(translateIndexes);
        fclose(extendedInputFile);
        remove_file(outputName, extendedInputExt);
    }

}



void read_operation(char *fullInputName, int line, char *cursor, int op, int *ICPtr) {
    
    cursor += strlen(operations[op]);

    /* check zero operands operations */
    if(op == rts || op == stop) {
        
        while(*cursor == ' ' || *cursor == '\t') cursor++;
        if(*cursor != '\n' && *cursor != '\0') {
            fprintf(stderr ,"Error in file '%s' at line %d: unexpected operand, the operation '%s' does not require any operands.\n", fullInputName, line, operations[op]);
            isError = 1;
            return;
        }
        
        add_operation_word(ICPtr, opCodes[op], 0, 0, 0, 0, functs[op], 1, 0, 0);
        return;
    }

    /* check for space after operation*/
    if(*cursor != ' ' && *cursor != '\t') {
        fprintf(stderr, "Error in file '%s' at line %d: missing space after the operation '%s'.\n", fullInputName, line, operations[op]);
        isError = 1;
        return;
    }
    while(*cursor == ' ' || *cursor == '\t') cursor++;
    
    
    /* handle one operand operations */
    if(op == clr || op == not || op == inc || op == dec || op == jmp || op == bne || op == jsr || op == red || op == prn) {
        decode_single_operand_operation(cursor, fullInputName, line, operations[op], op, ICPtr);
    }

    /* handle two operand operations */
    else {
        decode_two_operands_operation(cursor, fullInputName, line, operations[op], op, ICPtr);
    }
}

void add_operation_word(int *ICPtr, const int opcode, int source_addressing, int source_register, int target_addressing, int target_register, const int funct, int A, int R, int E) {
    
    /* create the operation word with the given arguments as a 32 bit int */
    uint32_t word_value = 0;
    word_value |= opcode << 18;
    word_value |= source_addressing << 16;
    word_value |= source_register << 13;
    word_value |= target_addressing << 11;
    word_value |= target_register << 8;
    word_value |= funct << 3;
    word_value |= A << 2;
    word_value |= R << 1;
    word_value |= E;

    /* make sure the word fits in 24 bits */
    word_value &= 0xFFFFFF;

    /* convert to word to hex string and save it in the code buffer */
    sprintf(codeImage[RELATIVE_IC(*ICPtr)], "%06x", word_value);
    (*ICPtr)++;
}



void decode_single_operand_operation(char *cursor, char *fullInputName, int line, const char *operation, int op, int *ICPtr) {
    long immediate_value; /* the immediate value of the operand (if exists) */
    int reg; /* the register number of the operand (if exists) */
    int oldError = isError; /* save isError value */
    
    /* check for missing operand */
    if(*cursor == '\n' || *cursor == '\0') {
        fprintf(stderr, "Error in file '%s' at line %d: missing operand, the operation '%s' requires exactly one operand.\n", fullInputName, line, operation);
        isError = 1;
        return;
    }

    /* handle immediate value operand */
    else if(*cursor == '#') {
        /* remove previous errors to check for extra operands*/
        isError = 0;

        /* check if the operation is compatible with immediate value operand */
        if(op != prn) {
            fprintf(stderr, "Error in file '%s' at line %d: illegal operand type, the operation '%s' does not accept operand in immediate addressing.\n", fullInputName, line, operation);
            isError = 1;
            return;
        }
        cursor++;

        /* extract the immediate value */
        immediate_value = extract_immediate_value(&cursor, fullInputName, line, "operation");
        if(isError) return;

        /* check for extra operands */
        if(*cursor != '\n' && *cursor != '\0') {
            fprintf(stderr, "Error in file '%s' at line %d: too many operands, the operation '%s' requires exactly one operand.\n", fullInputName, line, operation);
            isError = 1;
            return;
        }
        
        /* add the operation word and information word of the operand to the code buffer */
        add_operation_word(ICPtr, opCodes[op], 0, 0, immediate, 0, functs[op], 1, 0, 0); 
        add_information_word(ICPtr, immediate_value, 1, 0, 0);

        /* restore the previous error if exists */
        if(!isError) isError = oldError;
    }

    /* handle operand in relative addressing */
    else if(*cursor == '&') {

        /* check if the operation is compatible with operand in relative addressing */
        if(op != jmp && op != bne && op != jsr) {
            fprintf(stderr, "Error in file '%s' at line %d: illegal operand type, the operation '%s' does not accept operand in relative addressing.\n", fullInputName, line, operation);
            isError = 1;
            return;
        }
        cursor++;

        /* check if the operand is a valid label */
        if(!is_valid_label(&cursor, fullInputName, line)) {
            isError = 1;
            return;
        }

        /* check for extra operands */
        if(cursor != NULL) {
            while(*cursor == ' ' || *cursor == '\t') cursor++;
            if(*cursor != '\n' && *cursor != '\0') {
                fprintf(stderr, "Error in file '%s' at line %d: too many operands, the operation '%s' require only one operand.\n", fullInputName, line, operation);
                return;
            }
        }

        /* add operation word to the code buffer and save a place for the information word to be added later */
        add_operation_word(ICPtr, opCodes[op], 0, 0, relative, 0, functs[op], 1, 0, 0);
        (*ICPtr)++;
    }

    /* handle register operand */
    else if((reg = is_reg_operand(&cursor)) != -1){

        /* check if the operation is compatible with a register operand */
        if(op == jmp || op == bne || op == jsr) {
            fprintf(stderr, "Error in file '%s' at line %d: illegal operand type, the operation '%s' does not accept register operand.\n", fullInputName, line, operation);
            isError = 1;
            return;
        }
        
        /* check for extra operands */
        while(*cursor == ' ' || *cursor == '\t') cursor++;
        if(*cursor != '\n' && *cursor != '\0') {
            fprintf(stderr, "Error in file '%s' at line %d: too many operands, the operation '%s' require only one operand.\n", fullInputName, line, operation);
            isError = 1;
            return;
        }

        /* add operation word to the code buffer */
        add_operation_word(ICPtr, opCodes[op], 0, 0, direct_register, reg, functs[op], 1, 0, 0);
    }

    /* handle operand in direct addressing */
    else {

        /* check if the operand is a valid label */
        if(!is_valid_label(&cursor, fullInputName, line)) {
            isError = 1;
            return;
        }

        /* check for extra operands */
        if(cursor != NULL) {
            while(*cursor == ' ' || *cursor == '\t') cursor++;
            if(*cursor != '\n' && *cursor != '\0') {
                fprintf(stderr, "Error in file '%s' at line %d: too many operands, the operation '%s' require only one operand.\n", fullInputName, line, operation);
                return;
            }
        }

        /* add operation word to the code buffer and save a place for the information word to be added later */
        add_operation_word(ICPtr, opCodes[op], 0, 0, direct, 0, functs[op], 1, 0, 0);
        (*ICPtr)++;
    }
}

void add_information_word(int *ICPtr, long content, int A, int R, int E) {

    /* create the information word with the given arguments as a 32 bit int */
    uint32_t word_value = 0;
    word_value |= content << 3;
    word_value |= A << 2;
    word_value |= R << 1;
    word_value |= E;

    /* make sure the word fits in 24 bits */
    word_value &= 0xFFFFFF;

    /* convert to word to hex string and save it in the code buffer */
    sprintf(codeImage[RELATIVE_IC(*ICPtr)], "%06x", word_value); 
    (*ICPtr)++;
}

long extract_immediate_value(char **cursorPtr, char *fullInputName, int line, char *type) {
    long immediate_value; /* the immediate value as an integer */
    char *cursor = *cursorPtr; /* pointer to the operand in line */
    char *endPtr = cursor; /* pointer to the rest of the line after the integer */

    /* check for missing value */
    if(*cursor == '\n' || *cursor == '\0' || *cursor == ' ' || *cursor == '\t' || *cursor == ',') {
        fprintf(stderr, "Error in file '%s' at line %d: missing operand value.\n", fullInputName, line);
        isError = 1;
        return 0;
    }

    /* extract the immediate value to an integer */
    immediate_value = strtol(cursor, &endPtr, 10);
    
    /* check for an illegal immediate value */
    if(endPtr == cursor || (*endPtr != ' ' && *endPtr != '\t' && *endPtr != '\n' && *endPtr != '\0')) {
        if(*endPtr == ',') fprintf(stderr, "Error in file '%s' at line %d: the operand value '%s' is not an integer.\n", fullInputName, line, strtok(cursor, " \t\n"));
        else fprintf(stderr, "Error in file '%s' at line %d: the operand value '%s' is not an integer.\n", fullInputName, line, strtok(cursor, ", \t\n"));
        isError = 1;
        return 0;
    }
    
    /* if the immediate belongs to an operation */
    if(!strcmp(type, "operation")) {
        /* check for overflow/underflow in 21 bits */
        if(immediate_value > INT21_MAX || immediate_value < INT21_MIN) {
            if(immediate_value > INT21_MAX) fprintf(stderr, "Error in file '%s' at line %d: the operand value '%s' is too large, please enter a value between -2^20 and 2^20-1.\n", fullInputName, line, strtok(cursor, ", \t\n"));
            else fprintf(stderr, "Error in file '%s' at line %d: the operand value '%s' is too small, please enter a value between -2^20 and 2^20-1.\n", fullInputName, line, strtok(cursor, ", \t\n"));
            isError = 1;
            return 0;
        }
    }

    /* if the immediate belongs to a data directive */
    else {
        /* check for overflow/underflow in 24 bits */
        if(immediate_value > INT24_MAX || immediate_value < INT24_MIN) {
            if(immediate_value > INT24_MAX) fprintf(stderr, "Error in file '%s' at line %d: the operand value '%s' is too large, please enter a value between -2^24 and 2^24-1.\n", fullInputName, line, strtok(cursor, ", \t\n"));
            else fprintf(stderr, "Error in file '%s' at line %d: the operand value '%s' is too small, please enter a value between -2^24 and 2^24-1.\n", fullInputName, line, strtok(cursor, ", \t\n"));
            isError = 1;
            return 0;
        }
    }

    /* get the rest of the line after the integer */
    *cursorPtr = endPtr;
    while(**cursorPtr == ' ' || **cursorPtr == '\t') (*cursorPtr)++;

    /* return the immediate value */
    return immediate_value;
    
}

int is_valid_label(char **cursorPtr, char *fullInputName, int line) {
    char *cursor = *cursorPtr; /* pointer to the operand in line */
    char *label = strtok(cursor, " \t\n"); /* the operand as a string */
    int i;
    
    /* check for missing label */
    if(label == NULL) {
        fprintf(stderr, "Error in file '%s' at line %d: missing label name.\n", fullInputName, line);
        return 0;
    }

    /* check if the label is an operation*/
    for(i = 0; i < NUM_OPERATIONS; i++) {
        if(!strcmp(label, operations[i])) {
            fprintf(stderr, "Error in file '%s' at line %d: the label '%s' has illegal name, a label name can't be an operation name.\n", fullInputName, line, label);
            return 0;
        }
    }

    /* check if the label is a register */
    for(i = 0; i < NUM_REGISTERS; i++) {
        if(!strcmp(label, registers[i])) {
            fprintf(stderr, "Error in file '%s' at line %d: the label '%s' has illegal name, a label name can't be a register name.\n", fullInputName, line, label);
            return 0;
        }
    }

    /* check if the label is a macro */
    if(search_macro(macros, label) != NULL) {
        fprintf(stderr, "Error in file '%s' at line %d: the label '%s' has illegal name, a label name can't be a macro name.\n", fullInputName, line, label);
        return 0;
    }

    /* check that the first character of the label is an alphabetical character */
    if(!isalpha(*cursor)) {
        fprintf(stderr, "Error in file '%s' at line %d: the label '%s' has illegal name, the first letter of a label must be an alphabetic character.\n", fullInputName, line, label);
        return 0;
    }

    cursor++;

    /* check that the rest of the characters of the label are alphanumerical characters */
    for(i = 1; i < MAX_LABEL_LENGTH; i++) {
        if(*cursor == '\0') break;
        if(!isalnum(*cursor)) {
            fprintf(stderr, "Error in file '%s' at line %d: the label '%s' has illegal name, the label must contain only alphanumeric characters.\n", fullInputName, line, label);
            return 0;
        }
        cursor++;
    }
    
    /* check that the label length doesn't exceed the maximum length */
    if(*cursor != '\0') {
        fprintf(stderr, "Error in file '%s' at line %d: the label '%s' is too long, the maximum label length is %d characters.\n", fullInputName, line, label, MAX_LABEL_LENGTH);
        return 0;
    }
    
    /* get the rest of the line after the label */
    *cursorPtr = strtok(NULL, "");

    return 1;
}

int is_reg_operand(char **cursorPtr) {
    int i;
    char *cursor = *cursorPtr; /* pointer to the operand in line */

    /* iterate over all the registers */
    for(i = 0; i < NUM_REGISTERS; i++) {

        /* check if the label matches the current register in the loop */
        if(!strncmp(cursor, registers[i], strlen(registers[i]))) {
            if(*(cursor + strlen(registers[i])) == ' ' || *(cursor + strlen(registers[i])) == '\t' || *(cursor + strlen(registers[i])) == '\n' || *(cursor + strlen(registers[i])) == '\0') {
                *cursorPtr += strlen(registers[i]);
                return i;
            }
        } 
    }

    return -1;
}

void decode_two_operands_operation(char *cursor, char *fullInputName, int line, const char *operation, int op, int *ICPtr) {
    long source_immediate_value; /* the source operand immediate value (if exists) */
    long target_immediate_value; /* the target operand immediate value (if exists) */
    int source_reg; /* the source operand register number (if exists) */
    int source_addressing; /* the source addressing method */
    int target_reg; /* the target operand register number (if exists) */
    char *commaPtr = cursor; /* pointer to the comma in the line between the operands */
    int oldError = isError; /* save isError value */

    
    /* find the comma in the line and replace it with a blank */
    while(*commaPtr != ',' && *commaPtr != '\n' && *commaPtr != '\0') commaPtr++;
    if(*commaPtr != ',') {
        fprintf(stderr, "Error in file '%s' at line %d: missing comma, there must be a comma between the operands.\n", fullInputName, line);
        isError = 1;
        return;
    }
    *commaPtr = ' ';

    /* handle first operand */

    /* check for missing source operand */
    if(*cursor == '\n' || *cursor == '\0') {
        fprintf(stderr, "Error in file '%s' at line %d: missing source operand, the operation '%s' requires exactly two operands.\n", fullInputName, line, operation);
        isError = 1;
        return;
    }

    /* handle immediate value source operand */
    else if(*cursor == '#') {
        /* remove previous errors to check for comma erros or errors in the second operand*/
        isError = 0;

        /* check if the operation is compatible with immediate value source operand */
        if(op == lea) {
            fprintf(stderr, "Error in file '%s' at line %d: illegal operand type, the operation '%s' does not accept source operand in immediate addressing.\n", fullInputName, line, operation);
            isError = 1;
            return;
        }
        cursor++;

        /* extract the source operand immediate value */
        source_immediate_value = extract_immediate_value(&cursor, fullInputName, line, "operation");
        if(isError) return;
        
        source_reg = 0;
        source_addressing = immediate;

        /* restore the previous error if exists */
        if(!isError) isError = oldError;
    }

    /* handle register source operand */
    else if((source_reg = is_reg_operand(&cursor)) != -1) {

        /* check if the operation is compatible with a register source operand */
        if(op == lea) {
            fprintf(stderr, "Error in file '%s' at line %d: illegal operand type, the operation '%s' does not accept register operand.\n", fullInputName, line, operation);
            isError = 1;
            return;
        }
        
        source_addressing = direct_register;
    }

    /* handle source operand in direct addressing */
    else {

        /* check if the source operand is a valid label */
        if(!is_valid_label(&cursor, fullInputName, line)) {
            isError = 1;
            return;
        }

        source_reg = 0;
        source_addressing = direct;
    }

    /* check if the comma is between the operands */
    while(*cursor == ' ' || *cursor == '\t') cursor++;
    if(cursor < commaPtr) {
        fprintf(stderr, "Error in file '%s' at line %d: missing comma, there must be a comma between the operands.\n", fullInputName, line);
        isError = 1;
        return;
    }

    /* handle second operand */
    
    /* check for missing target operand */
    if(*cursor == '\n' || *cursor == '\0') {
        fprintf(stderr, "Error in file '%s' at line %d: missing target operand, the operation '%s' requires exactly two operands.\n", fullInputName, line, operation);
        isError = 1;
        return;
    }

    /* handle immediate value target operand */
    else if(*cursor == '#') {
        /* remove previous errors to check for extra operands*/
        isError = 0;

        /* check if the operation is compatible with immediate value target operand */
        if(op != cmp) {
            fprintf(stderr, "Error in file '%s' at line %d: illegal operand type, the operation '%s' does not accept target operand in immediate addressing.\n", fullInputName, line, operation);
            isError = 1;
            return;
        }
        cursor++;

        /* extract the target operand immediate value */
        target_immediate_value = extract_immediate_value(&cursor, fullInputName, line, "operation");
        if(isError) return;

        /* check for extra operands */
        if(*cursor != '\n' && *cursor != '\0') {
            fprintf(stderr, "Error in file '%s' at line %d: too many operands, the operation '%s' requires exactly two operands.\n", fullInputName, line, operation);
            isError = 1;
            return;
        }
        
        
        /* add the words encodings of the whole line */
        add_operation_word(ICPtr, opCodes[op], source_addressing, source_reg, immediate, 0, functs[op], 1, 0, 0);
        if(source_addressing == immediate) add_information_word(ICPtr, source_immediate_value, 1, 0, 0);
        else if(source_addressing == direct) (*ICPtr)++;
        add_information_word(ICPtr, target_immediate_value, 1, 0, 0);

        /* restore the previous error if exists */
        if(!isError) isError = oldError;
    }

    /* handle register target operand */
    else if((target_reg = is_reg_operand(&cursor)) != -1) {

        /* check for extra operands */
        while(*cursor == ' ' || *cursor == '\t') cursor++;
        if(*cursor != '\n' && *cursor != '\0') {
            fprintf(stderr, "Error in file '%s' at line %d: too many operands, the operation '%s' require exactly two operands.\n", fullInputName, line, operation);
            isError = 1;
            return;
        }
        
        /* add the words encodings of the whole line */
        add_operation_word(ICPtr, opCodes[op], source_addressing, source_reg, direct_register, target_reg, functs[op], 1, 0, 0);
        if(source_addressing == immediate) add_information_word(ICPtr, source_immediate_value, 1, 0, 0);
        else if(source_addressing == direct) (*ICPtr)++;
    }

    /* handle target operand in direct addressing */
    else {

        /* check if the target operand is a valid label */
        if(!is_valid_label(&cursor, fullInputName, line)) {
            isError = 1;
            return;
        }

        /* check for extra operands */
        if(cursor != NULL) {
            while(*cursor == ' ' || *cursor == '\t') cursor++;
            if(*cursor != '\n' && *cursor != '\0') {
                fprintf(stderr, "Error in file '%s' at line %d: too many operands, the operation '%s' require exactly two operands.\n", fullInputName, line, operation);
                return;
            }
        }

        /* add the words encodings of the whole line */
        add_operation_word(ICPtr, opCodes[op], source_addressing, source_reg, direct, 0, functs[op], 1, 0, 0);
        if(source_addressing == immediate) add_information_word(ICPtr, source_immediate_value, 1, 0, 0);
        else if(source_addressing == direct) (*ICPtr)++;
        (*ICPtr)++;
    }
}

void read_directive(char *fullInputName, int line, char *cursor, int directive, int *DCPtr) {
    long immediate_number; /* current immediate integer (if data directive)*/
    char *commaPtr = cursor; /* pointer to the comma in the line between the operands */
    char *tmp; /* temporary pointer */
    char *endLabel; /* pointer to the end of the label */
    int labelLength; /* the length of the label including null pointer */
    char *labelName = NULL; /* the label name */
    labelNode *label = NULL; /* a temporary label */
    int oldError = isError; /* save isError value */

    cursor += strlen(directives[directive]);
    
    /* check for space after the directive */
    if(*cursor != ' ' && *cursor != '\t') {
        fprintf(stderr, "Error in file '%s' at line %d: missing space after the directive '%s'.\n", fullInputName, line, directives[directive]);
        isError = 1;
        return;
    }

    while(*cursor == ' ' || *cursor == '\t') cursor++;

    /* handle data directive */
    if(directive == data) {
        /* remove previous errors to check for errors later in the line */
        isError = 0;
        do {

            /* find the comma in the line and replace it with a blank */
            while(*commaPtr != ',' && *commaPtr != '\n' && *commaPtr != '\0') {
                commaPtr++;
            }
            if(*commaPtr == ',' && commaPtr != cursor) *commaPtr = ' ';
            
            /* extract the current immediate value */
            immediate_number = extract_immediate_value(&cursor, fullInputName, line, "data");
            if(isError) return;

            /* check that the comma was in the right place */
            if(cursor < commaPtr) {
                fprintf(stderr, "Error in file '%s' at line %d: missing comma, there must be a comma between every subsequent operands.\n", fullInputName, line);
                isError = 1;
                return;
            }

            /* make sure the word fits in 24 bits */
            immediate_number &= 0xFFFFFF;

            /* convert to word to hex string and save it in the data buffer */
            sprintf(dataImage[*DCPtr], "%06x", immediate_number);
            
            (*DCPtr)++;
        }
        while(*commaPtr != '\n' && *commaPtr != '\0');

         /* restore the previous error if exists */
        if(!isError) isError = oldError;
    }

    /* handle string directive*/
    else if(directive == string) {

        /* check for beginning quotion mark*/
        if(*cursor != '"') {
            fprintf(stderr, "Error in file '%s' at line %d: missing quotion marks.\n", fullInputName, line);
            isError = 1;
            return;
        }
        cursor++;

        /* iterate over each character and store it in the data buffer */
        while(*cursor != '"' && *cursor != '\n' && *cursor != '\0') {
            sprintf(dataImage[*DCPtr], "%06x", *cursor);
            (*DCPtr)++;
            cursor++;
        }

        /* store a null terminator in the data buffer*/
        sprintf(dataImage[*DCPtr], "%06x", '\0');
        (*DCPtr)++;

        /* check for ending quotion mark*/
        if(*cursor != '"') {
            fprintf(stderr, "Error in file '%s' at line %d: missing quotion marks.\n", fullInputName, line);
            isError = 1;
            return;
        }
        cursor++;

        /* check for extra operands */
        if(*cursor != '\n' && *cursor != '\0') {
            fprintf(stderr, "Error in file '%s' at line %d: too many operands, the directive '%s' require only one operand.\n", fullInputName, line, directives[directive]);
            isError = 1;
            return;
        }

    }

    /* handle entry or extern directives */
    else {
        
        tmp = cursor;
        endLabel = cursor;
        while(*endLabel != ' ' && *endLabel != '\t' && *endLabel != '\n' && *endLabel != '\0') endLabel++;

        if(!is_valid_label(&tmp, fullInputName, line)) {
            isError = 1;
            return;
        }
            
        /* copy the label name into label */
        labelLength = endLabel - cursor + 1;
        labelName = (char *)malloc((labelLength) * sizeof(char));
        strncpy(labelName, cursor, labelLength);
        
        label = search_label(labels, labelName);
        /* check if the label is already declared and it's invalid to declare it again */
        if(label != NULL && (directive == extrn || (label->type != _code && label->type != _data))) {
            fprintf(stderr, "Error in file '%s' at line %d: the label '%s' is already declared.\n", fullInputName, line, labelName);
            isError = 1;
            return;
        }

        /* handle entry directive */
        if(directive == entry) {

            /* if label is not defined add it to the labels list, otherwise update the type to include entry */
            if(label == NULL) append_label(&labels, labelName, 0, _unidentified_entry);
            else {
                if(label->type == _code) label->type = _code_entry;
                else label->type = _data_entry;
            }
        }

        /* add the label if its an external label */
        else append_label(&labels, labelName, 0, _external);
        
        
        cursor = tmp;
        /* check for extra operands */
        if(cursor != NULL) {
            while(*cursor == ' ' || *cursor == '\t') cursor++;
            if(*cursor != '\n' && *cursor != '\0') {
                fprintf(stderr, "Error in file '%s' at line %d: too many operands, the directive '%s' require only one operand.\n", fullInputName, line, directives[directive]);
                return;
            }
        }

    }
    
}

void read_label(char *fullInputName, int line, char *cursor, int *ICPtr, int *DCPtr) {
    char *colonPtr = strchr(cursor, ':'); /* ptr to the colon after the label decleration */
    char *tmp = cursor; /* temporary cursor for the function is_valid_label() */
    int labelLength; /* length of the label including null terminator */
    char *labelName = NULL; /* label name */
    labelNode *label = NULL; /* temporary label */
    int i;
    
    /* check that the colon exists*/
    if(colonPtr == NULL) {
        fprintf(stderr, "Error in file '%s' at line %d: missing colon after label decleration .\n", fullInputName, line);
        isError = 1;
        return;
    }

    /* check for space between the label and the colon */
    while(*tmp != ' ' && *tmp != '\t' && *tmp != ':') tmp++;
    if(tmp != colonPtr) {
        fprintf(stderr, "Error in file '%s' at line %d: illegal space between label decleration and colon.\n", fullInputName, line);
        isError = 1;
        return;
    }

    /* remove it so the is_valid_label function won't include it in the label */
    *colonPtr = '\0';
    tmp = cursor;
    /* check that the label has valid synthax */
    if(!is_valid_label(&tmp, fullInputName, line)) {
        isError = 1;
        return;
    }


    /* copy the label name into label */
    labelLength = colonPtr - cursor + 1;
    labelName = (char *)malloc((labelLength) * sizeof(char));
    strncpy(labelName, cursor, labelLength);

    label = search_label(labels, labelName);
    /* check if the label is already declared */
    if(label != NULL && label->type != _unidentified_entry) {
        fprintf(stderr, "Error in file '%s' at line %d: the label '%s' is already declared.\n", fullInputName, line, labelName);
        isError = 1;
        return;
    }

    cursor = colonPtr + 1;
    while(*cursor == ' ' || *cursor == '\t') cursor++;
    
    /* check if the label is an operation label */
    for(i = 0; i < NUM_OPERATIONS; i++) {
        if(!strncmp(cursor, operations[i], strlen(operations[i]))) {
            tmp = cursor + strlen(operations[i]);
            /* check if the operation is not a prefix of a label */
            if(*tmp == ' ' || *tmp == '\t' || *tmp == '\n' || *tmp == '\0') {
                /* if the label is entry*/
                if(label != NULL) {
                    label->type = _code_entry;
                    label->value = *ICPtr;
                }
                else append_label(&labels, labelName, *ICPtr, _code);

                /* read the rest of the line */
                read_operation(fullInputName, line, cursor, i, ICPtr);
                return;
            }   
        }
    }

    /* check if the label is a data directive label */
    if(!strncmp(cursor, directives[data], strlen(directives[data]))) {
        /* if the label is entry*/
        if(label != NULL) {
            label->type = _data_entry;
            label->value = *DCPtr;
        }
        else append_label(&labels, labelName, *DCPtr, _data);
        read_directive(fullInputName, line, cursor, data, DCPtr);
    }

    /* check if the label is a string directive label */
    else if(!strncmp(cursor, directives[string], strlen(directives[string]))) {
         /* if the label is entry*/
         if(label != NULL) {
            label->type = _data_entry;
            label->value = *DCPtr;
        }
        else append_label(&labels, labelName, *DCPtr, _data);
        read_directive(fullInputName, line, cursor, string, DCPtr);
    }

    /* handle invalid operation or directive after label decleration */
    else {
        fprintf(stderr, "Error in file '%s' at line %d: invalid operation or directive after label decleration.\n", fullInputName, line);
        isError = 1;
    }


}

void update_data_labels() {
    labelNode *label = labels;
    while(label != NULL) {
        if(label->type == _data || label->type == _data_entry) label->value += ICF;
        label = label->next;
    }
}