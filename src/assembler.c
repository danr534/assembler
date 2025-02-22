#include "first_pass.h"
#include "pre_assembler.h"
#include "utils.h"
#include "globals.h"

int main(int argc, char *argv[]) {
    int currentIndex; /* the index of the current assembly file */
    char *buffer; /* the pointer of the current assembly file */
    char *inputName; /* the name of the current assembly file (without extensions)*/
    char *outputName; /* the name of the current output files (without extensions)*/
    FILE *inputFile; /* current assembly file pointer */
    long inputFileSize; /* the size of the input file in bytes */
    FILE *extendedInputFile;

    /* check that there is at least one command line argument */
    if(argc < 2) {
        fprintf(stderr, "Error: Missing command line argument, please enter at least one assembly file.\n");
        exit(1);
    }
    
    /* iterate over each assembly file and compile it */
    for(currentIndex = 1; currentIndex < argc; currentIndex++) {
        inputName = combine_strings(INPUT_DIR, argv[currentIndex]);

        /* open the current assembly input file */
        inputFile = open_file(inputName, inputExt, readMode);
        if(inputFile == NULL) continue;

        /* convert the file to a string */
        buffer = get_file_content(inputFile, inputName, inputExt, &inputFileSize);
        if(buffer == NULL) continue;

        /* expend the macros and save the result */
        outputName = combine_strings(OUTPUT_DIR, argv[currentIndex]);
        expand_macros(buffer, inputFileSize, outputName);
        if(isError) continue;

        extendedInputFile = open_file(outputName, extendedInputExt, readMode);
        first_pass(extendedInputFile, inputName);
    }

    return 0;
}

