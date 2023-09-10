
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "enums.h"
#include "get_line_info_structs.h"
#include "get_line_info.h"
#include "convertion_structs.h"
#include "convertion_phase.h"
#include "create_output.h" 
/*ABOUT OUTPUT PHASE
we created several functions to convert the converted data into base 64 
and to print its value to the proper output files.
basic stuff of fopen and arithmetics..
*/

/**
 * @brief this function print the entry/extern list information- 
 * were they were used or defined (entry- were they were defined,
 * extern- were they were used as parameters)
 * basically- we run linearly on the list end orint the entry and extern names and their adress
 * deffenition/usage(accordingly to their type(entry/extern) as explained above)
 */

void make_labels_output(Label_List *outputList, const char * fileName){
    FILE *outputFile;
    label_node *current_ex_node;

    outputFile = fopen(fileName, "w");
    if (outputFile == NULL) {
        printf("Failed to create the file.\n");
        return;
    }
    
    current_ex_node= outputList->head;
    while (current_ex_node!=NULL){
        fprintf(outputFile,"%s %d\n", current_ex_node->label_name, current_ex_node->index);
        current_ex_node=current_ex_node->next;
    }
    fclose(outputFile);
}
/**
 * @brief the main function that calls the semi functions that converts to base 64 and prints the output
 */

void print_output(const char * fileName, Converted_File *convertedFile){
    FILE *outputFile;
    char *outputFileName =create_file_name(fileName, ".ob");
    char *entryFile =create_file_name(fileName, ".ent");
    char *externFile =create_file_name(fileName, ".ext");

    outputFile = fopen(outputFileName, "w");
    if (outputFile == NULL) {
        printf("Failed to create the file.\n");
        return;
    }
    make_labels_output(convertedFile->labelListsGroup->entryList, entryFile);
    make_labels_output(convertedFile->labelListsGroup->externList, externFile);
    print_counters(outputFile,convertedFile->convertedAarrays->command_c, convertedFile->convertedAarrays->inst_c);
    make_ob_file( outputFile ,convertedFile->convertedAarrays);
    free(outputFileName);
    free(externFile);
    free(entryFile);
    outputFileName=NULL;
    externFile=NULL;    
    entryFile=NULL;
    fclose(outputFile);
}
/**
 * @brief calls the convert to base 64 function at the right order- firts the command array- then the inst array
 */
void make_ob_file(FILE *outputFile, Converted_Arrays *convertedArrays){
    print_order_table(outputFile, convertedArrays->command_bit_array, convertedArrays->command_c);
    print_order_table(outputFile, convertedArrays->inst_bit_array, convertedArrays->inst_c);       
}
/**
 * @brief a simple function tp translate  a short to base 64
 */
unsigned short convert_to_relevant_char(unsigned short word){
    if(word>=0 &&word<=25)
        word+=65;
    else if (word>=26 &&word<=51)
        word+=71;
    else if (word>=52 &&word<=61)
        word-=4;
    else if (word==62)
        word=43;
    else if (word==63) 
        word=47;

    return word;
}
void print_counters(FILE *outputFile, int command_c, int inst_c){
    fprintf(outputFile, "%d %d\n", command_c,inst_c);
}

/**
 * @brief a simple function that extract the left and right part of a number to be translated seperatly into 2
 * base 64 chars
 */
void print_order_table(FILE *outputFile, short *bit_array, int counter){ 
    unsigned short leftWord=0;
    unsigned short rightWord=0;
    int i;
    for(i=0; i< counter; i++){
        unsigned short convertLeft= 0xFC0;
        unsigned short convertRight= 0x3F;
        rightWord =(bit_array[i] & convertRight);
        leftWord = (bit_array[i] & convertLeft) ;
        leftWord =leftWord>>6;
        
        leftWord= convert_to_relevant_char(leftWord);
        rightWord= convert_to_relevant_char(rightWord); 
        fprintf(outputFile, "%c%c\n", leftWord,rightWord);
    }
}
/**
 * @brief allows us to create the relevant output files names (am/ob/ent/ext)
 */
char* create_file_name(const char *fileName, char *extention){
    char *result =(char*)malloc(sizeof(char)*(strlen(fileName)+strlen(extention)+1));
    strcpy(result, fileName);
    strcat(result, extention);
    return result;
}