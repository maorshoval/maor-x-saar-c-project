
#ifndef CREATE_OUTPUT_H
#define CREATE_OUTPUT_H
#include "enums.h"
#include "get_line_info_structs.h"
#include "get_line_info.h"
#include "convertion_structs.h"
#include "convertion_phase.h"

void print_output(const char * fileName, Converted_File *convertedFile);
void make_labels_output(Label_List *outputList, const char * fileName);
void make_ob_file(FILE *outputFile,Converted_Arrays *convertedAarrays);
unsigned short convert_to_relevant_char(unsigned short word);
void print_counters(FILE *outputFile, int command_c, int inst_c);
void print_order_table(FILE *outputFile, short *bit_array, int counter);
char* create_file_name(const char *fileName, char *extention);

#endif