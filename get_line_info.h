
#ifndef GET_LINE_INFO_H
#define GET_LINE_INFO_H
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "enums.h"
#include "get_line_info_structs.h"

/*functions*/
const Command* set_comm_array();
const Instruction* set_inst_array();
const char* skip(const char *current, const char *generic);
boolean empty_or_comment_line(const char *input);
void check_line_length(char **input);
void add_error(Error *error, char *add, const char *current);

const char *label_deffinition_properties(const char *start, const char *end, Error *error, const Command *comm_array, const Instruction *inst_array, Order_Type lineType);
boolean label_inner_propreties(const char *start, const char *label, Error *error, const Command *comm_array, const Instruction *inst_array);
const char* create_label( const char *start, const char* end);
const char *colon_vs_start(const char *firstLetter);
const char *win_big(const char *firstLetter, const char *start_of_order);/*colon_vs_order_vs_space*/
const Line_Info* get_line_info(const char *input, int lineI, const Command *comm_array, const Instruction *inst_array, boolean *flag_errors_detected);
const char* find_order(const Command *comm_array,const Instruction *inst_array, const char *current, int *this_order, Order_Type *type);
boolean is_legit(const char *current,const  char *curr_order);
boolean is_only_spaces(const char *current,size_t len);
boolean is_dot_before_inst(const char *input, const char *curr_order, Error *error, Order_Type type);
const char* get_dataP(const char **currentPtr, Error *error, int ** nums, int *sizeN);
const char* inner_get_dataP(const char **next, const char **prev,  int *i, Error *error, int ** nums, int *sizeN, Param_Type *paramType);

const char* check_comma(const char *current, Error *error, boolean shouldBe);
boolean check_endP(const char *current, Error *error);
const char* check_start(const char *input, const char *current);
Error* set_error(const char*input);

const Param* get_command_param(const char **currentPtr, Error *error, Poss_P_Type PPT,const Command *comm_array,const Instruction *inst_array);
const char* check_regP(const char **currentPtr, Error *error, Param_Type *paramType, int *regP);
const char* check_numP(const char **currentPtr, Error *error, Param_Type *paramType, int *numP);
const char *label_param_propreties(const char **currentPtr, Error *error, Param_Type *paramType, const Command *comm_array, const Instruction *inst_array,const  char **labelP);
const Param* create_param(const Param_Type paramType, int *regP, int *numP, const char *labelP, int sizeN);

const char* check_prior_param(const char *current, int orderN, Error *error);
const char* str_vs_data_param(Order_Type type, const char **currentPtr, Error *error, int **nums, int *sizeN);
const char* get_strP(const char **currentPtr, Error *error, int **nums, int *sizeN);
int* create_strP( const char *start, const char *end, int *sizeN);
void freeParam(const Param *thisP);
void print_error(Error *error, int lineI);
const Line_Info* create_line_info( Order_Type type,const char* label, short orderN,const Param* sourceParam,const Param* targetParam );
void free_line(const Line_Info *thisLine);
const char* check_between_order_and_param(const char *current,const Command *comm_array, const Instruction *inst_array,Order_Type type, int orderN, Error *error );
const char* check_param_overflow(const char *current , const Command *comm_array, int orderN, Error *error);
const char* get_pre_param_info(const char *input, const char **label, const Command *comm_array, const Instruction *inst_array, int *orderN, Order_Type *type, Error *error);
boolean is_line_syntax_legit(Error *error, int lineI,const  Param *sourceParam,const Param *targetParam);
const Param* get_comm_source_param(const char **currentPtr, const Command *comm_array, const Instruction *inst_array,Error *error, int orderN);
const Param* get_comm_target_param(const char **currentPtr, const Command *comm_array, const Instruction *inst_array,Error *error, int orderN);

#endif



