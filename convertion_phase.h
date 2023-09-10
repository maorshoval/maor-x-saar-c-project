#ifndef CONVERTION_PHASE_H
#define CONVERTION_PHASE_H
#include "enums.h"
#include "get_line_info_structs.h"
#include "convertion_structs.h"


Converted_Arrays* create_converted_arrays();
Label_Lists_Group* creat_label_lists_group();
void semi_convertion(const Line_Info *thisLine, Converted_Arrays *convertedArrays, Label_Lists_Group *labelListsGroup ,int lineI);
void insert_to_bit_array(const Param *param, short **bit_array, int *bit_array_size, int *counter);
void inner_insert_to_bit_array(const Param *param, short **bit_array, int *bit_array_size, int *counter);
int define_label_index(Order_Type type, int inst_c,int command_c);
Label_List* new_label_list();
label_node* new_label_node();
void set_label_node(label_node *curr_node, Order_Type type,const char *name, int index_decision);
void add_to_list(Label_List *entryList, Order_Type type,const char *name, int index_decision);
char* copy_label_name(const char *name);
void set_label_into_trie(Order_Type type, const char *insertLabel, Label_Trie_Node *labelTrieRoot, Label_List *trieList, int index_decision,int lineI);

void set_label_params_adress(Converted_Arrays *convertedArrays, Label_Lists_Group *labelListsGroup);
void edit_missing_label_cell(short **bit_array, int missing_label_param_index, int counter, int label_param_def_index);
void edit_external_label_cell(short **bit_array, int missing_label_param_index);

void label_param_into_SRLL(const Param *thisParam,Converted_Arrays *convertedArrays, Label_Lists_Group *LabelListsGroup);
void check_entry_label_definition(Label_Lists_Group *labelListsGroup, Converted_Arrays *convertedArrays);
void clean_itteration(const Line_Info **thisLine, char **input, int *lineI);
void clean_input_phase(const Command *comm_array, const Instruction *inst_array, char *input, FILE *file);
void clean_convertion_phase(Converted_Arrays *convertedArrays, Label_Lists_Group *labelListsGroup);
void clean_converted_file(Converted_File *this);
Converted_File* create_converted_file(Converted_Arrays *convertedAarrays, Label_Lists_Group *listsGroup);

void allocate_command_bit_array(int num_len, short **bit_array, int *bit_array_size, int *counter);
void add_to_command_array(short **command_bit_array,int *command_c,int  base);

void bit_translator_first_word (const Line_Info *line_input, short **command_bit_array,int *command_array_length, int *command_c);
void bit_translator_general(const Line_Info *line_input, Converted_Arrays *CA, Label_Lists_Group *LLG, short **command_bit_array,int *command_array_length, int *command_c);
void make_word_both_reg(const Line_Info *line_input,short **command_bit_array,int *command_array_length, int *command_c);
void sourceP_reg_translation(const Line_Info *line_input,short **command_bit_array,int *command_array_length, int *command_c);
void sourceP_num_translation(const Line_Info *line_input,short **command_bit_array,int *command_array_length, int *command_c);
void targetP_reg_translation(const Line_Info *line_input,short **command_bit_array,int *command_array_length, int *command_c);
void targetP_num_translation(const Line_Info *line_input,short **command_bit_array,int *command_array_length, int *command_c);


void free_label_list(Label_List *thisList);
Label_Trie_Node* create_label_trie_node();
void insert_label_trie_node(Label_Trie_Node* root, const char* key, label_node* label);
label_node* search_label(Label_Trie_Node* root, const char* key);
void free_label_trie_Node(Label_Trie_Node* node);
void free_label_trie(Label_Trie_Node* root);
#endif
