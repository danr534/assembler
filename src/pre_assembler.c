#include "pre_assembler.h"

int *translateIndexes;
int numLines;
int isError;
macroNode *macros = NULL; 

const char *operations[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec", "jmp", "bne", "jsr", "red", "prn", "rts", "stop"};
const char *directives[] = {".data", ".string", ".entry", ".extern"};

const static char *macro_start = "mcro"; /* start of a macro decleration*/
const static char *macro_end = "mcroend"; /* end of a macro decleration */

/**
 * Verifies that the macro is valid and add it to the macros list including name, content, starting line index and the total lines inside it
 * @param cursorPtr pointer to the cursor
 * @param as_counter line counter of the input file
 * @param am_counter line counter of the output file
 * @param macros list of the macros found until now
 * @param inputFilename the full name of the input file
 */
void add_macro(char **cursorPtr, int *as_counter, int *am_counter, macroNode **macros, char *inputFilename);

/**
 * Checks if the macro's name is a saved word or if it contains unnecessary characters
 * @param cursor buffer pointer of the current line
 * @param startIndex the index of the line with the macro defintion
 * @param inputFilename the full name of the input file
 * @return 1 if an error was found, 0 otherwise
 */
int name_error(char *cursor, int startIndex, char *inputFilename);

/**
 * Checks if the line is ending the macro decleration
 * @param cursor buffer pointer of the current line
 * @param startIndex the index of the line with the macro defintion
 * @param totalLines the total lines inside the macro until now
 * @param inputFilename the full name of the input file
 * @param am_counter line counter of the output file
 * @return 1 if a the line is a valid ending of the macro, 0 otherwise
 */
int is_macro_end(char **cursorPtr, int startIndex, int totalLines, char *inputFilename, int *am_counter);

/**
 * maps a macro call in the input file to its corresponding macro content/block in the output file
 * @param outputFile the output file pointer
 * @param macro the macro that was found
 * @param am_counter line counter of the output file
 * @param as_counter line counter of the input file
 * @param cursorPtr pointer to the cursor
 */
void replace_macro(FILE *outputFile, macroNode *macro, int *am_counter, int *as_counter, char **cursorPtr);

/**
 * copiese the current line from the input file to the output file
 * @param outputFile the output file pointer
 * @param cursorPtr pointer to the cursor
 * @param as_counter line counter of the input file
 * @param am_counter line counter of the output file
 */
void add_line(FILE *outputFile, char **cursorPtr, int *as_counter, int *am_counter);


void expand_macros(char *buffer, long inputFileSize, char *filename) {
    char *cursor = buffer; /* buffer pointer of the current line */
    int as_counter = 1; /* line counter of the input file */
    int am_counter = 1; /* line counter of the output file */
    
    macroNode *macro = NULL; /* temporary macro pointer */
    FILE *outputFile = open_file(filename, extendedInputExt, writeMode); /* output file pointer */
    char *inputFilename = combine_strings(filename, inputExt); /* full input file name*/
    long numLinesUpperBound = inputFileSize * inputFileSize; /* upper bound for the number of lines in the output file */
    /* initilize external variables */
    isError = 0;
    translateIndexes = (int *)malloc(numLinesUpperBound * sizeof(int));

    /* check if memory allocation failed */
    if(translateIndexes == NULL) {
        fprintf(stderr, "Error in file '%s': ", filename);
        perror("");
        isError = 1;
    }

    else {
        /* scan until reached the end of input file buffer */
        while(*cursor != '\0') {
            /* skip white characters */
            while(*cursor == ' ' || *cursor == '\t') cursor++;
            if(*cursor == ' ' || *cursor == '\t') cursor++;

            /* check if the line is a macro decleration or macro call or regular line and call the corresponding function */
            if(!strncmp(cursor, macro_start, strlen(macro_start))) add_macro(&cursor, &as_counter, &am_counter, &macros, inputFilename);
            else if((macro = search_macro(macros, cursor))) replace_macro(outputFile, macro, &am_counter, &as_counter, &cursor);
            else add_line(outputFile, &cursor, &as_counter, &am_counter);
        }
    }
   
    numLines = am_counter;

    /* free dynamic memory */
    free(buffer);

    /* close the output file and remove it if an error was found */
    fclose(outputFile);
    if(isError) remove_file(filename, extendedInputExt);

}

void add_macro(char **cursorPtr, int *as_counter, int *am_counter, macroNode **macros, char *inputFilename) {
    char *name; /* the name of the macro */
    char *nameStart; /* pointer to the name of the macro in the buffer */
    int nameLen = 0; /* the length of macro name */

    char *content; /* the content inside the macro */
    char *contentStart; /* pointer the content of the macro in the buffer */
    int contentLen = 0; /* the length of the content of the macro */

    int totalLines = 0; /* the total lines of the macro's content */
    int startIndex = *as_counter; /* the line index of the macro definition */

    *cursorPtr += strlen(macro_start) + 1; /* skip to the macro name */
    
    /* check if the macro is an instruction or directive */
    if(name_error(*cursorPtr, startIndex, inputFilename)) {
        isError = 1;
        while(**cursorPtr != '\n' && **cursorPtr != '\0') (*cursorPtr)++;
        if(**cursorPtr == '\n') {
            (*cursorPtr)++;
            (*as_counter)++;
        }
        else (*am_counter)--;
        return;
    }

    nameStart = *cursorPtr;
    
    /* save the macro's name */
    while(**cursorPtr != ' ' && **cursorPtr != '\t' && **cursorPtr != '\n') (*cursorPtr)++;
    nameLen = *cursorPtr - nameStart;
    name = (char *)malloc((nameLen + 1) * sizeof(char));
    strncpy(name, nameStart, nameLen);
    name[nameLen] = '\0';

    while(**cursorPtr != '\n') (*cursorPtr)++;
    contentStart = ++(*cursorPtr);

    /* scan each line until a valid macro ending line */
    while(1) {
        if(is_macro_end(cursorPtr, *as_counter, totalLines, inputFilename, am_counter)) break;
        while(**cursorPtr != '\n' && **cursorPtr != '\0') {
            (*cursorPtr)++;
            contentLen++;
        }
        contentLen++;
        totalLines++;
        if(**cursorPtr == '\0') break;
        (*cursorPtr)++;
        
    }

    /* save the macro's content */
    content = (char *)malloc((contentLen + 1) * sizeof(char));
    strncpy(content, contentStart, contentLen);
    content[contentLen] = '\0';

    *as_counter += totalLines + 2; /* add the number of lines of the entire macro to the input line counter */
    
    append_macro(macros, name, content, startIndex, totalLines); /* add the macro to the macros list */
    
}

int name_error(char *cursor, int startIndex, char *inputFilename) {
    int i;
    char tmp;
    /* check if the macro's name is an instruction */
    for(i = 0; i < NUM_OPERATIONS; i++) {
        if(!strncmp(cursor, operations[i], strlen(operations[i]))) {
            tmp = *(cursor + strlen(operations[i]));
            if(tmp == ' ' || tmp == '\t' || tmp == '\n' || tmp == '\0' ) {
                fprintf(stderr, "Error in file '%s' at line %d: macro name is an instruction.\n", inputFilename, startIndex);
                return 1;
            }
        }
    }

    /* check if the macro's name is a directive */
    for(i = 0; i < NUM_DIRECTIVES; i++) {
        if(!strncmp(cursor, directives[i], strlen(directives[i]))) {
            tmp = *(cursor + strlen(directives[i]));
            if(tmp == ' ' || tmp == '\t' || tmp == '\n' || tmp == '\0') {
                fprintf(stderr, "Error in file '%s' at line %d: macro name is a directive.\n", inputFilename ,startIndex);
                return 1;
            }
        }
    }
    
    /* check if the line contains any more non white characters */
    while(*cursor != ' ' && *cursor != '\t' && *cursor != '\n' && *cursor != '\0') cursor++;
    while(*cursor == ' ' || *cursor == '\t') cursor++;
    if(*cursor != '\n' && *cursor != '\0') {
        fprintf(stderr, "Error in file '%s' at line %d: extraneous text after macro definition.\n", inputFilename, startIndex);
        return 1;
    }
    
    return 0;
}

int is_macro_end(char **cursorPtr, int startIndex, int totalLines, char *inputFilename, int *am_counter) {
    char *cursor = *cursorPtr;

    /* skip white characters*/
    while(*cursor == ' ' || *cursor == '\t') cursor++;
    if(*cursor == ' ' || *cursor == '\t') cursor++;

    /* check if the line prefix is a valid macro ending */
    if(!strncmp(cursor, macro_end, strlen(macro_end))) {
        cursor += strlen(macro_end);
        while(*cursor == ' ' || *cursor == '\t') cursor++;
        /* check if the line contains any more non white characters */
        if(*cursor != '\n' && *cursor != '\0') {
            fprintf(stderr, "Error in file '%s' at line %d: extraneous text after macro end.\n", inputFilename ,startIndex + totalLines + 1);
            isError = 1;
            return 0;
        }
        *cursorPtr = cursor;
        if(**cursorPtr == '\n') (*cursorPtr)++;
        else (*am_counter)--;
        return 1;
    }
    return 0;
}


void replace_macro(FILE *outputFile, macroNode *macro, int *am_counter, int *as_counter, char **cursorPtr) {
    int i;
    char *tmp = *cursorPtr + strlen(macro->name);

    while(*tmp == ' ' || *tmp == '\t') tmp++;
    /* if the line contains extra characters treat it as a regular line and not a call to macro */
    if(*tmp != '\n' && *tmp != '\0') {
        add_line(outputFile, cursorPtr, as_counter, am_counter);
        return;
    }
    
    /* update the translateIndexes array with the new macro lines */
    for(i = 0; i < macro->totalLines; i++) translateIndexes[*am_counter + i] = macro->startIndex + 1 + i;
    *am_counter += macro->totalLines - 1;

    /* go to the start of the next line  */
    while(**cursorPtr != '\n' && **cursorPtr != '\0') (*cursorPtr)++;
    if(**cursorPtr == '\n') {
        (*cursorPtr)++;
        (*as_counter)++;
        (*am_counter)++;
    }
    else {
        macro->content[strlen(macro->content) - 1] = '\0';
    }

    /* copy the macro content to the output file */
    fprintf(outputFile, "%s", macro->content);
}

void add_line(FILE *outputFile, char **cursorPtr, int *as_counter, int *am_counter) {

    /* copy the line to the output file*/
    while(**cursorPtr != '\n' && **cursorPtr != '\0') {
        fputc(**cursorPtr, outputFile);
        (*cursorPtr)++;
    }

    /* update translateIndexes array with the new line */
    translateIndexes[*am_counter] = *as_counter;

    /* go to the start of the next line */
    if(**cursorPtr == '\n') {
        fputc(**cursorPtr, outputFile);
        (*cursorPtr)++;
        (*as_counter)++;
        (*am_counter)++;
    }
}