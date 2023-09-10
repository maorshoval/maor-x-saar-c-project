#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "enums.h"
#include "macro.h"
#include "get_line_info_structs.h"
#include "get_line_info.h"
#include "convertion_structs.h"
#include "convertion_phase.h"
#include "create_output.h"
/*ABOUT HTE PROJECT-
we focused on constructiong relevant structs and enums that will allow us to work at the wost efficiently way we
could think of. we used trie to represent labels and macros and seach through them efficiently.
we used severl 'decision trees' like functions, to choose corectly how to handle each order type and param type.
we tried to make the functions as readable as possible- so overall- the project is devided to 
big functions that calls semi functions, sored of a 'babuska doll'.
the main structs we used-
line info- a struct representing the current relevant line information- its order type, params, labels (if exists).
tries (as explained above)
lists- mainly to represent labels and labels location - used for holding and finding the proper
adress of a label (as were it was set as parameter or were it was defined).
converted arrays structs- a struct that holds the command table and instruction table, and its counters (to be printed at the ob output file).
for more info about the struct- see the structs headers; more explanation about their usage at the relevant functions.

AUTHORS: Saar nurflus, Maor Shoval
date: 18/08/2023

*/

int assembler(char *fileName);

/**
 * @brief main funcrion- calss preprocesor function named macro expand- then calss assemlber function to proccess the file
 * @param 
 */

int main(int argc, char *argv[]) {
    int i = 0;
    char *filename = (char *)malloc(sizeof(char) * 50);
    print_beginnig();
    
    for (i = 1; i < argc; i++) {
        printf("\nstaring macro expands for file %s\n", argv[i]);
        
        macro_expand(argv[i]);
        
        strcpy(filename, argv[i]);
        
        assembler(filename);
        
        memset(filename, '\0', MAX_FILE_NAME);
    }
    printf("----END OF PROGRAM----\n");

return 0;

}
/**
 * @brief assembler- calss the function that proccess the assembler
 * @param file name for the current itteration
 */

int assembler(char *fileName){
    int lineI=0;
    boolean flag_erros_detected=false;
    boolean flag_produce_output=true;
    const Command* comm_array= set_comm_array();
    const Instruction* inst_array = set_inst_array();
    char *input = (char*)calloc((MAX_LINE_LENGTH+1), sizeof(char));
    const Line_Info *thisLine=NULL;
    Converted_Arrays *convertedArrays= create_converted_arrays();
    Label_Lists_Group *labelListsGroup= creat_label_lists_group();
    Converted_File* convertedFile=NULL;
    char *expendedMacros =create_file_name(fileName, ".am");
    FILE *file = fopen(expendedMacros, "r");
    if (file == NULL) {
        printf("the file %s failed to open\n", expendedMacros);
        return -1;
    }
    printf("\n------------------------\n reading from file %s.am\n------------------------\n", fileName);
    while (fgets(input,MAX_LINE_LENGTH, file)!=NULL){
 
        check_line_length(&input);
        if(empty_or_comment_line((const char *)input)==false){
            thisLine=get_line_info(input, lineI, comm_array,inst_array, &flag_erros_detected);
            semi_convertion(thisLine, convertedArrays, labelListsGroup, lineI);
        } 
        
        if (flag_erros_detected==true)
            flag_produce_output=false;
        
        clean_itteration(&thisLine, &input, &lineI);

    }
    /*end of while loop*/
    convertedFile= create_converted_file(convertedArrays, labelListsGroup);
    if (flag_produce_output==false)
        printf("--ERRORS WHERE DITECTED IN FILE; WILL NOT PRODUCE TRANSLATION--\n");
    
    else {
        check_entry_label_definition(labelListsGroup, convertedArrays);
        set_label_params_adress(convertedArrays,labelListsGroup);   
        printf("--FINISHED CONVERTION PHASE FOR FILE %s --\n", fileName);
        print_output(fileName ,convertedFile);
        printf("--OUTPUT FILES FOR %s ARE READY--\n", fileName);
    }
    

    clean_converted_file(convertedFile);
    printf("\n--FINISHED READING FILE %s.am--\n",fileName);
    printf("\n------------------------\n");
    free(expendedMacros);
    expendedMacros=NULL;
    return 0;
}

