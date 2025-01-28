#include "pre_assembler.h"
#include "utils.h"
#include "globals.h"

int main(int argc, char *argv[]) {
    int currentIndex; /* the index of the current assembly file */
    char *buffer; /* the pointer of the current assembly file */
    char *inputName; /* the name of the current assembly file (without extensions)*/
    char *outputName; /* the name of the current output files (without extensions)*/
    FILE *file; /* current assembly file pointer */
    long inputFileSize; /* the size of the input file in bytes */

    /* check that there is at least one command line argument */
    if(argc < 2) {
        printf("Error: Missing command line argument, please enter at least one assembly file.\n");
        exit(1);
    }

    /* iterate over each assembly file and compile it */
    for(currentIndex = 1; currentIndex < argc; currentIndex++) {
        inputName = combine_strings(INPUT_DIR, argv[currentIndex]);

        /* open the current assembly input file */
        file = open_file(inputName, inputExt, readMode);
        if(file == NULL) continue;

        /* convert the file to a string */
        buffer = get_file_content(file, inputName, inputExt, &inputFileSize);
        if(buffer == NULL) continue;

        /* expend the macros and save the result */
        outputName = combine_strings(OUTPUT_DIR, argv[currentIndex]);
        expand_macros(buffer, inputFileSize, outputName);
        if(isError) continue;
        
    }
    return 0;
}

