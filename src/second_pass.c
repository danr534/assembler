#include "second_pass.h"
#include "utils.h"

/**
 * Encodes the labels addresses in the operation in the current line to the code buffer.
 * @param objFile the obj file pointer.
 * @param extFile the external file pointer.
 * @param fullInputName the full name of the input file.
 * @param line line counter in the input file.
 * @param cursor pointer of the current line (skips blank characters at the beginning).
 * @param op the operation number in respect to the operations names.
 * @param ICPtr pointer to IC.
 */
void encode_operation(FILE *objFile, FILE *extFile, char *fullInputName, int line, char *cursor, int op, int *ICPtr);

/**
 * Encodes the label in an operation line with a single operand to the code buffer.
 * @param objFile the obj file pointer.
 * @param extFile the external file pointer.
 * @param cursor pointer to the operand in the line.
 * @param fullInputName the full name of the input file.
 * @param line the line number in the input file.
 * @param operation the operation name.
 * @param ICPtr pointer to IC.
 */
void encode_single_operand_operation(FILE *objFile, FILE *extFile, char *cursor, char *fullInputName, int line, const char *operation, int op, int *ICPtr);

/**
 * Encodes the label in an operation line with two operands to the code buffer.
 * @param objFile the obj file pointer.
 * @param extFile the external file pointer.
 * @param cursor pointer to the operand in the line.
 * @param fullInputName the full name of the input file.
 * @param line the line number in the input file.
 * @param operation the operation name.
 * @param ICPtr pointer to IC.
 */
void encode_two_operands_operation(FILE *objFile, FILE *extFile, char *cursor, char *fullInputName, int line, const char *operation, int op, int *ICPtr);

/**
 * Encodes the entry directive in the current line in the data buffer and checks for errors with entry and external labels.
 * @param entFile the entry file pointer.
 * @param extFile the external file pointer.
 * @param fullInputName the full name of the input file.
 * @param line line counter in the input file.
 * @param cursor pointer of the current line (skips blank characters at the beginning).
 * @param directive the directive number in respect to the operations names
 */
void encode_directive(FILE *entFile, FILE *extFile, char *fullInputName, int line, char *cursor, int directive);

/**
 * Encodes a line with a label at the beginning.
 * @param objFile the obj file pointer.
 * @param extFile the external file pointer.
 * @param fullInputName the full name of the input file.
 * @param line line counter in the input file.
 * @param cursor pointer of the current line (skips blank characters at the beginning).
 * @param ICPtr pointer to IC.
 */
void encode_label(FILE *objFile, FILE *extFile, char *fullInputName, int line, char *cursor, int *ICPtr);

/**
 * Writes the entry labels and their corresponding values to the entry file.
 * @param entFile the entry file pointer.
 */
void write_entries(FILE *entFile);

void second_pass(FILE *extendedInputFile, char *inputName, char *outputName) {
    FILE *objFile = open_file(outputName, objExt, writeMode); /* obj file pointer */
    FILE *entFile = open_file(outputName, entryExt, writeMode); /* entry file pointer */
    FILE *extFile = open_file(outputName, externalExt, writeMode); /* external file pointer */
    char *cursor = (char *)malloc(MAX_LINE_LENGTH * sizeof(char)); /* pointer of the current line */
    int counter = 1; /* line counter in the expanded input file */
    int IC = INITIAL_CODE_ADDRESS; /* counter of the code encoded words */
    int DC; /* counter of the data encoded words */
    char *fullInputName = combine_strings(inputName, inputExt); /* the full name of the input file */
    int is_read; /* a boolean variable that is 1 if the line is already read and 0 otherwise */
    char *tmp; /* temporary pointer to check that the operation is not a prefix of a label */
    int i;
    
    /* write the first line in the obj file specifying the lengths of the code and data buffers */
    fprintf(objFile, "%7d %-7d\n", ICF - INITIAL_CODE_ADDRESS, DCF);


    /* iterate over each line in the expended input file and encode it to the code buffer and write the entry and external labels in their files */
    while(fgets(cursor, MAX_LINE_LENGTH, extendedInputFile) != NULL) {
        is_read = 0;
        while(*cursor == ' ' || *cursor == '\t') cursor++; /* skip over white characters */

        /* check if the current line is an operation */
        for(i = 0; i < NUM_OPERATIONS; i++) {
            if(!strncmp(cursor, operations[i], strlen(operations[i]))) {
                tmp = cursor + strlen(operations[i]);
                if(*tmp == ' ' || *tmp == '\t' || *tmp == '\n' || *tmp == '\0') {
                    encode_operation(objFile, extFile, fullInputName, translateIndexes[counter], cursor, i, &IC);
                    is_read = 1;
                }
            }
        }
        
        /* check if the current line is a directive */
        for(i = 0; i < NUM_DIRECTIVES; i++) {
            
            if(!strncmp(cursor, directives[i], strlen(directives[i]))) {
                
                encode_directive(entFile, extFile, fullInputName, translateIndexes[counter], cursor, i);
                is_read = 1;
            }
        }

        /* ensure the line isn't empty or a comment */
        if(*cursor != ';' && *cursor != '\n' && *cursor != '\0' && is_read == 0) {
            encode_label(objFile, extFile, fullInputName, translateIndexes[counter], cursor, &IC);
        }
        counter++;
    }

    /* write the code buffer data to the obj file */
    for(IC = INITIAL_CODE_ADDRESS; IC < ICF; IC++) {
        fprintf(objFile, "%07d %s\n", IC, codeImage[RELATIVE_IC(IC)]);
    }

    /* write the data buffer data to the obj file */
    for(DC = 0; DC < DCF; DC++) {
        fprintf(objFile, "%07d %s\n", ICF + DC, dataImage[DC]);
    }

    /* write the entry labels and their values */
    write_entries(entFile);

    /* free the dynamic memory */
    free_labels_list(labels);
    free(translateIndexes);
    free(cursor);
    free(fullInputName);


    /* if there is an error remove all the output files */
    if(isError) {
        fclose(extendedInputFile);
        fclose(objFile);
        fclose(entFile);
        fclose(extFile);
        remove_file(outputName, extendedInputExt);
        remove_file(outputName, objExt);
        remove_file(outputName, entryExt);
        remove_file(outputName, externalExt);
        return;
    }

    /* close the all the open files */
    fclose(extendedInputFile);
    fclose(objFile);
    
    /* if the entry file is empty, remove it */
    if(fgetc(entFile) == EOF) {
        fclose(entFile);
        remove_file(outputName, entryExt);
    }
    else fclose(entFile);
    

    /* if the external file is empty remove it */
    if(fgetc(extFile) == EOF) {
        fclose(extFile);
        remove_file(outputName, externalExt);
    }
    else fclose(extFile);

}

void encode_operation(FILE *objFile, FILE *extFile, char *fullInputName, int line, char *cursor, int op, int *ICPtr) {

    cursor += strlen(operations[op]);

    /* skip the operation encoding */
    (*ICPtr)++;

    /* check zero operands operations */
    if(op == rts || op == stop) return;

    /* skip space after operation */
    while(*cursor == ' ' || *cursor == '\t') cursor++;
    
    
    /* handle one operand operations */
    if(op == clr || op == not || op == inc || op == dec || op == jmp || op == bne || op == jsr || op == red || op == prn) {
        encode_single_operand_operation(objFile, extFile, cursor, fullInputName, line, operations[op], op, ICPtr);
    }

    /* handle two operands operations */
    else {
        encode_two_operands_operation(objFile, extFile, cursor, fullInputName, line, operations[op], op, ICPtr);
    }

}

void encode_single_operand_operation(FILE *objFile, FILE *extFile, char *cursor, char *fullInputName, int line, const char *operation, int op, int *ICPtr) {
    labelNode *label = NULL; /* temporary label */
    long jumpDist; /* relative distance for relative addressing */

    /* remove comma */
    strtok(cursor, ",");

    /* skip immediate value operation word */
    if(*cursor == '#') (*ICPtr)++;

    /* handle operand in relative addressing */
    else if(*cursor == '&') {

        cursor++;

        /* check if the operand label exists */
        if((label = search_label(labels, cursor)) == NULL) {
            fprintf(stderr, "Error in file '%s' at line %d: the label '%s' isn't defined.\n", fullInputName, line, strtok(cursor, " \t\n"));
            isError = 1;
            return;
        }

        /* check that the operand label is not external */
        if(label->type == _external) {
            fprintf(stderr, "Error in file '%s' at line %d: the external label '%s' can't be used with relative addressing.\n", fullInputName, line, strtok(cursor, " \t\n"));
            isError = 1;
            return;
        }

        /* check that the operand label is not an undefined entry label */
        if(label->type == _unidentified_entry) {
            fprintf(stderr, "Error in file '%s' at line %d: the entry label '%s' is not defined.\n", fullInputName, line, strtok(cursor, " \t\n"));
            isError = 1;
            return;
        }

        /* add information word to the code buffer */
        jumpDist = label->value - *ICPtr + 1;
        add_information_word(ICPtr, jumpDist, 1, 0, 0);
        
    }

    /* handle operand in direct addressing */
    else if(is_reg_operand(&cursor) == -1) {

        /* check if the operand label exists */
        if((label = search_label(labels, cursor)) == NULL) {
            fprintf(stderr, "Error in file '%s' at line %d: the label '%s' isn't defined.\n", fullInputName, line, strtok(cursor, " \t\n"));
            isError = 1;
            return;
        }

        /* check that the operand label is not an undefined entry label */
        if(label->type == _unidentified_entry) {
            fprintf(stderr, "Error in file '%s' at line %d: the entry label '%s' is not defined.\n", fullInputName, line, strtok(cursor, " \t\n"));
            isError = 1;
            return;
        }

        /* handle external label */
        if(label->type == _external) {
            /* write the reference of the external label in the external file */
            fprintf(extFile, "%s %07d\n", label->name, *ICPtr);

            /* add information word to the code buffer */
            add_information_word(ICPtr, label->value, 0, 0, 1);    
       }

       /* handle internal label*/
       else {
            /* add information word to the code buffer */
            add_information_word(ICPtr, label->value, 0, 1, 0);
       }

    }

}

void encode_two_operands_operation(FILE *objFile, FILE *extFile, char *cursor, char *fullInputName, int line, const char *operation, int op, int *ICPtr) {

    int oldError = isError; /* save isError value */

    /* remove previous errors to check for errors in the second operand */
    isError = 0; 

    /* handle first operand */
    encode_single_operand_operation(objFile, extFile, cursor, fullInputName, line, operations[op], op, ICPtr);
    if(isError) return;

    /* get to the second operand */
    cursor = strtok(NULL, " \t\n");

    /* handle second operand */
    encode_single_operand_operation(objFile, extFile, cursor, fullInputName, line, operations[op], op, ICPtr);

    /* restore the previous error if exists */
    if(!isError) isError = oldError;
}

void encode_directive(FILE *entFile, FILE *extFile, char *fullInputName, int line, char *cursor, int directive) {

    labelNode *label = NULL; /* temporary label */
    
    cursor += strlen(directives[directive]);
    while(*cursor == ' ' || *cursor == '\t') cursor++;

    /* handle entry directive */
    if(directive == entry) {
        label = search_label(labels, cursor);

        /* check the label is defined */
        if(label->type == _unidentified_entry) {
            fprintf(stderr, "Error in file '%s' at line %d: the entry label '%s' is not defined.\n", fullInputName, line, strtok(cursor, " \t\n"));
            isError = 1;
            return;
        }

    }

    /* handle external directive */
    else if(directive == extrn) {
        label = search_label(labels, cursor);

        /* check that the label is not an entry label */
        if(label->type == _code_entry || label->type == _data_entry) {
            fprintf(stderr, "Error in file '%s' at line %d: the label '%s' is an entry label and it can't also be an external label.\n", fullInputName, line, strtok(cursor, " \t\n"));
            isError = 1;
        }

    }
}

void encode_label(FILE *objFile, FILE *extFile, char *fullInputName, int line, char *cursor, int *ICPtr) {
    int i;
    while(*cursor != ':') cursor++;
    cursor++;
    while(*cursor == ' ' || *cursor == '\t') cursor++;

    /* check if the label is an operation label */
    for(i = 0; i < NUM_OPERATIONS; i++) {
        if(!strncmp(cursor, operations[i], strlen(operations[i]))) {    
            encode_operation(objFile, extFile, fullInputName, line, cursor, i, ICPtr);
            return;
        }   
    }


}

void write_entries(FILE *entFile) {
    labelNode *label = labels;

    /* write all the code entry labels to the entry file */
    while(label != NULL) {
        if(label->type == _code_entry) fprintf(entFile, "%s %07d\n", label->name, label->value);
        label = label->next;
    }

    label = labels;
    /* write all the data entry labels to the entry file */
    while(label != NULL) {
        if(label->type == _data_entry) fprintf(entFile, "%s %07d\n", label->name, label->value);
        label = label->next;
    }
    
}