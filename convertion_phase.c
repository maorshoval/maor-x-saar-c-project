

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "enums.h"
#include "get_line_info_structs.h"
#include "get_line_info.h"
#include "convertion_structs.h"
#include "convertion_phase.h"
/*ABOUT CONVERTION PHASE- we used several structs to allow us create the convertion at the most 
efficient way possible.
the main idea was to firstly run and translate all the non labeld parameters, and to set their translation
into the relevant arrays (one for command order and one for instructions (string and data) orders).
the arrays goal is to hold the converted orders and  parameters representation so we could later translate them
easly to base 64.
the data/strings params are already represented as numbers, so we just inserted them into the inst array.
the command order though- needed cerful translation for several senarios, so we used a decision tree to do so.
as for the labels- this was a bit tricky as well- we set laebel lists to represent the locations and name of 
different labels-
we had one list of the label defenitions,
one for the entry labels - so we would be able to set the .ent file properly,
one for the extern labels -so we would be able to set the .ext file properly,
and one for the labels that are set as parameters in commands' orders.
the lists allowed as to check on what location a label was defined;
and if it was an entry/extern label- where was it defined and used.
the list for the labels that were used as parmeters- we used a list named second run labels list 
to save at what location they were used as params- and later convert their actuall defentition location
as the adress of this label and therfore the paramter number representation.
at the end of the convertion phase- we return a struct that holds all the relevant lists and arrays,
as well as the relevant counter of the command array and inst array. all are important to the 
printing output phase.
*/

/**
 * @brief creates the trie node that will allow as to search labels through the program
 */
Label_Trie_Node* create_label_trie_node() {
    Label_Trie_Node* node = (Label_Trie_Node*)malloc(sizeof(Label_Trie_Node));
    int i;
    if (node) {
        node->isEndOfWord = false;
        node->label_data = NULL;

        for (i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

/**
 * @brief Inserts a label node into a label trie structure.
 * 
 * This function inserts a label node into a trie data structure using characters from the provided key.
 * The label node is associated with the given key and is stored at the leaf node corresponding to the key's characters.
 * 
 * @param root Pointer to the root node of the label trie.
 * @param key The key string used to determine the insertion path in the trie.
 * @param label Pointer to the label node to be inserted and associated with the key.
 */
void insert_label_trie_node(Label_Trie_Node* root, const char* key, label_node* label) {
    int i;
    int index;
    Label_Trie_Node* curr = root;
    for (i = 0; key[i] != '\0'; i++) {
        index = (key[i] >= 'a' && key[i] <= 'z') ? key[i] - 'a' :
                (key[i] >= 'A' && key[i] <= 'Z') ? key[i] - 'A' :
                (key[i] >= '0' && key[i] <= '9') ? key[i] - '0' + 26 : -1;
        if (index != -1 && curr->children[index] == NULL) {
            curr->children[index] = create_label_trie_node();
        }
        if (index == -1) {
            continue; /* Skip non-alphanumeric characters */
        }
        curr = curr->children[index];
    }
    curr->isEndOfWord = true;
    curr->label_data = label;
}
/**
 * @brief the fucntion that allows us to search if a given label exist already in the trie or not
 *
    Searches for a label node associated with the given key in a label trie structure.
 * 
 * This function searches for a label node associated with the provided key in a trie data structure.
 * It traverses the trie based on the characters in the key and returns the label node associated with
 * the key if found in this node the label content will be found.
 * 
 * @param root Pointer to the root node of the label trie.
 * @param key The key string to search for in the trie.
 * @return A pointer to the label node associated with the key, or NULL if the key is not found in the trie.
 */
label_node* search_label(Label_Trie_Node* root, const char* key) {
    Label_Trie_Node* curr = root;
    int i;
    int index;
    if (key==NULL)  
        return NULL;

    for (i = 0; key[i] != '\0'; i++) {
        if (key[i] == '\r' || key[i] == '\n') {
            break;
        }
        index = (key[i] >= 'a' && key[i] <= 'z') ? key[i] - 'a' :
                (key[i] >= 'A' && key[i] <= 'Z') ? key[i] - 'A' :
                (key[i] >= '0' && key[i] <= '9') ? key[i] - '0' + 26 : -1;
        if (index == -1 || curr->children[index]== NULL) {
            return NULL;
        }
        curr = curr->children[index];
    }
    if (curr != NULL && (curr->isEndOfWord)==true)
        return curr->label_data;
    else
        return NULL;
}


void free_label_trie_node(Label_Trie_Node* node) {
    int i;
    if (node == NULL) {
        return;
    }

    for (i = 0; i < ALPHABET_SIZE; i++) {
        free_label_trie_node(node->children[i]);
    }
    free(node);
}


void free_label_trie(Label_Trie_Node* root) {
    free_label_trie_node(root);
}
/**
 * @brief creating the converted array struct
 */
Converted_Arrays* create_converted_arrays(){
    Converted_Arrays *this=(Converted_Arrays*)malloc(sizeof(Converted_Arrays));
    
    this->inst_c=0;
    this->command_c=0;
    this->inst_bit_array=(short*)malloc(sizeof(short)*10);
    this->command_bit_array=(short*)malloc(sizeof(short)*10);
    this->inst_bit_array_size=10;
    this->command_bit_array_size=10;
    
    return this;
}
/**
 * @brief creating the label list group struct
 */
Label_Lists_Group* creat_label_lists_group(){
    Label_Lists_Group *this=(Label_Lists_Group*)malloc(sizeof(Label_Lists_Group));
    
    this->labelTrieRoot=create_label_trie_node();
    this->trieList= new_label_list();
    this->entryList= new_label_list();    
    this->second_run_labels_list= new_label_list();
    this->externList= new_label_list();
    
    return this;
}
/**
 * @brief a very important function that do the first phase of convertion.
 * it uses many semi functions to convert and insert accordingly.
 * it reads the current line info , and convert the relevant information to its 
 * numeric representation. after we have it as numbers, we can easly convert it to base 64/
 * as goes for the get line info function- this function works as a decision tree-
 * each type of order recieve a different interpretation and location.
 * command orders and its params are inserted to the command array (called command bit array);
 * data/string orders' parameters are inserted into the isntruction array (named inst bit array);
 * those 2 arrays are short type, we convert the relevant information and translate it , then we insert them to those array.
 * the insertion is linear- first command and its params are conerted and inserted first, then second command and its 
 * parametrs, and so on.. same goes for the data/string instructions parameters
 * 
 */
void semi_convertion(const Line_Info *thisLine, Converted_Arrays *convertedArrays, Label_Lists_Group *labelListsGroup , int lineI){
    Order_Type type;
    int index_decision;
    const char* lineLabel;
    
    if(thisLine==NULL)
        return;

    type= thisLine->type;
    lineLabel = thisLine->label;
    index_decision= define_label_index(thisLine->type, convertedArrays->inst_c,convertedArrays->command_c);

    if (lineLabel!=NULL)
        set_label_into_trie(type, lineLabel, labelListsGroup->labelTrieRoot, labelListsGroup->trieList, index_decision, lineI);
    
    if (type==EXTERN){
        const char* externalLabel=thisLine->sourceP->label;
        set_label_into_trie(type, externalLabel,labelListsGroup->labelTrieRoot,labelListsGroup->trieList, index_decision, lineI);
    }
    else if(type==ENTRY){
        const char *internalLabel=thisLine->sourceP->label;
        add_to_list(labelListsGroup->entryList, type, internalLabel, index_decision); /*for future check- to verify that indeed all entry labels were properly defined with command/data/string in the file*/
    }
    else if(type==DATA ||type==STRING)
       insert_to_bit_array(thisLine->sourceP, &(convertedArrays->inst_bit_array), &(convertedArrays->inst_bit_array_size), &(convertedArrays->inst_c));
    

    else if (type==COMMAND){ 
        bit_translator_first_word (thisLine, &(convertedArrays->command_bit_array), &(convertedArrays->command_bit_array_size), &(convertedArrays->command_c));
        bit_translator_general(thisLine,convertedArrays, labelListsGroup, &(convertedArrays->command_bit_array),  &(convertedArrays->command_bit_array_size), &(convertedArrays->command_c));   
    }
}
/**
 * @brief inserting a converted number to the array
 */
void insert_to_bit_array(const Param *param, short **bit_array, int *bit_array_size, int *counter){
    if (param->num_len + *counter > *bit_array_size){
        *bit_array = (short*)realloc(*bit_array, sizeof(short) * (param->num_len + (*counter) + 10));
        *bit_array_size=param->num_len + *counter+10;
    }
    inner_insert_to_bit_array(param, bit_array, bit_array_size, counter);
 }
void inner_insert_to_bit_array(const Param *param, short **bit_array, int *bit_array_size, int *counter){
    int i=0;
    if (param->type==LABEL){
        (*bit_array)[*counter]=0;
        (*counter)++;
    }

    else for (i=0;i<param->num_len;i++){
        (*bit_array)[*counter]=(short)((param->num)[i]);
        (*counter)++;
    }
}

int define_label_index(Order_Type type, int inst_c,int command_c){
    if (type==ENTRY || type==EXTERN)
        return 0;
    else if (type==COMMAND)
        return command_c;
        else return inst_c;
}

Label_List* new_label_list(){
    Label_List *result =(Label_List*)malloc(sizeof(Label_List));
    result->head=NULL;
    result->tail=NULL;
    return result;
}
label_node* new_label_node(){
    label_node *result =(label_node*)malloc(sizeof(label_node));
    result->label_name=NULL;
    result->next=NULL;
    return result; 
}
/**
 * @brief a function that add a label to a a  rlevant list
 */
void add_to_list(Label_List *thisList, Order_Type type,const char *name, int index_decision){
    label_node *curr_node = new_label_node();
    set_label_node(curr_node, type, name, index_decision);
    
    if (thisList->head == NULL && thisList->tail == NULL) {
        thisList->head = curr_node;
        thisList->tail = curr_node;
    } else {
        thisList->tail->next = curr_node;
        thisList->tail = curr_node;
    }
    curr_node->next = NULL; 
}

void set_label_node(label_node *curr_node, Order_Type type,const char *name, int index_decision){
    curr_node->type = type;
    if (curr_node->label_name != NULL) {
        free((void *)(curr_node->label_name));
    }
    curr_node->label_name = copy_label_name(name);
    curr_node->index = index_decision;
}

char* copy_label_name(const char *name){
    char *result= (char*)malloc(sizeof(char)*(strlen(name)+1));
    strcpy(result, name);
    return result;
}


void free_label_list(Label_List *thisList){
    label_node* current;
    label_node* toBeFree;
    if(thisList==NULL)
        return;
    current = thisList->head;
    
    while (current != NULL) {
        toBeFree=current;
        current = current->next;
        if (toBeFree!=NULL){
        free((void*)(toBeFree->label_name));
        toBeFree->label_name=NULL;
        free(toBeFree); 
        toBeFree=NULL;     
        }                  
    }
    thisList->head = NULL;
    thisList->tail = NULL;
}

/**
 * @brief inserting labels that were defined in the file into the trie array- so we could search it 
 * efficiently later
 */
void set_label_into_trie(Order_Type type, const char *insertLabel, Label_Trie_Node *labelTrieRoot, Label_List *trieList, int index_decision, int lineI){
    if(search_label(labelTrieRoot,insertLabel)!=NULL)
        printf("warning- at line %d: trying to redifine the label %s. this label was already defined erliear in the file- will cause corrupted translation\n",lineI , insertLabel);
    
    else{
        add_to_list(trieList,type,insertLabel, index_decision);
        insert_label_trie_node(labelTrieRoot, insertLabel, trieList->tail);
    }
}

/**
 * @brief inserting the label parameters into the second run labels list.
 * we decided that all the labels set as parameters (of command orders) into a 
 * list. than-after reading the entire file- we move linearly over this list and find the proper 
 * adress of this label- sored of as explained in the second run assembler in the project defenitions
 */
void label_param_into_SRLL(const Param *thisParam, Converted_Arrays *convertedArrays, Label_Lists_Group *LabelListsGroup){
   
    if (thisParam!=NULL && thisParam->type==LABEL){
        add_to_list(LabelListsGroup->second_run_labels_list, NONE, thisParam->label, convertedArrays->command_c);
        allocate_command_bit_array(1, &(convertedArrays->command_bit_array), &(convertedArrays->command_bit_array_size), &(convertedArrays->command_c));
        add_to_command_array(&(convertedArrays->command_bit_array), &(convertedArrays->command_c), 0);  
    }
}

/**
 * @brief this part we check the entries defenition location in file- where the actual label information was defined.
 * we run through the entry list and search at the trie the label name- than, through the pointer of the current label
 * in the trie- we find the relevant position
 */
void check_entry_label_definition(Label_Lists_Group *labelListsGroup, Converted_Arrays *convertedArrays){
    
    label_node *current_node= labelListsGroup->entryList->head;
    label_node *found;
    while (current_node!=NULL){
    found= search_label(labelListsGroup->labelTrieRoot,current_node->label_name);
    if (found==NULL)
        printf("warning- at label : %s, label was defined as entry but was not implemented in the file! can cause undefined behavior\n", current_node->label_name);
    else if(found!=NULL && found->type==COMMAND)
        current_node->index=FIRST_MEMORY_CELL + found->index;
    
    else if(found!=NULL && (found->type==DATA ||found->type==STRING))
        current_node->index=FIRST_MEMORY_CELL + convertedArrays->command_c + found->index;
    
    current_node=current_node->next;
    }
}

/**
 * @brief the function that set the proper label param adress - 
 * if a command order had a label- the number shpuld represent its decleration location- were the information it holds is located in the converted file
 */
void set_label_params_adress(Converted_Arrays *convertedArrays, Label_Lists_Group *labelListsGroup){
    label_node *missing_label_param= labelListsGroup->second_run_labels_list->head;
    label_node *label_param_def;
    while (missing_label_param!=NULL){
        label_param_def= search_label(labelListsGroup->labelTrieRoot, missing_label_param->label_name);
        if (label_param_def==NULL)
            printf("warning- at label : %s; label was used as a parameter but was not defined in file! will cause corrupted translation\n", missing_label_param->label_name);
        else if(label_param_def!=NULL && (label_param_def->type==COMMAND))
            edit_missing_label_cell(&(convertedArrays->command_bit_array), missing_label_param->index, 0, label_param_def->index);
        else if(label_param_def!=NULL && (label_param_def->type==DATA || label_param_def->type==STRING)){
            edit_missing_label_cell(&(convertedArrays->command_bit_array), missing_label_param->index, convertedArrays->command_c, label_param_def->index);
        }
        else if(label_param_def!=NULL && label_param_def->type==EXTERN){
            add_to_list(labelListsGroup->externList, EXTERN, missing_label_param->label_name, FIRST_MEMORY_CELL + (missing_label_param->index));
            edit_external_label_cell(&(convertedArrays->command_bit_array), missing_label_param->index);
        }
    missing_label_param=missing_label_param->next;
    }
}
/**
 * @brief a semi function to insert to the relevant label param cell in the converted command bit array
 */
void edit_missing_label_cell(short **bit_array, int missing_label_param_index, int counter, int label_param_def_index) {
    (*bit_array)[missing_label_param_index] = (short)(FIRST_MEMORY_CELL + counter + label_param_def_index);
    (*bit_array)[missing_label_param_index] = ((*bit_array)[missing_label_param_index]) << 2;
    (*bit_array)[missing_label_param_index] |= 2;
}
void edit_external_label_cell(short **bit_array, int missing_label_param_index) {
    (*bit_array)[missing_label_param_index] = 1;

}

            
Converted_File* create_converted_file(Converted_Arrays *convertedAarrays, Label_Lists_Group *labelListsGroup){
    Converted_File *this= (Converted_File*)malloc(sizeof(Converted_File));
    this->convertedAarrays=convertedAarrays;
    this->labelListsGroup=labelListsGroup;
    return this;
}      
/**
 * @brief a function to clean the current line in file itteration
 */
 void clean_itteration(const Line_Info **thisLine, char **input, int *lineI){
        free_line(*thisLine);
        *thisLine=NULL;
        
        memset(*input, '\0', MAX_LINE_LENGTH);
        (*lineI)++; 
}
/**
 * @brief cleaning and freeing the entire file and syntax-check relevant structs
 */
void clean_input_phase(const Command *comm_array, const Instruction *inst_array, char *input, FILE *file){
    free((void*)comm_array);
    free((void*)inst_array);
    free(input);
   
    comm_array=NULL;    
    inst_array=NULL;
    fclose(file);
    }

void clean_convertion_phase(Converted_Arrays *convertedArrays, Label_Lists_Group *labelListsGroup) {
    if (labelListsGroup==NULL)
        return;
    free_label_trie(labelListsGroup->labelTrieRoot);
    free_label_list(labelListsGroup->trieList);
    free_label_list(labelListsGroup->entryList);
    free_label_list(labelListsGroup->second_run_labels_list); 
    free_label_list(labelListsGroup->externList);
    
    if (convertedArrays==NULL)
        return;
    free(convertedArrays->inst_bit_array);
    convertedArrays->inst_bit_array = NULL;
    
    free(convertedArrays->command_bit_array);
    convertedArrays->command_bit_array = NULL;
    
    free(labelListsGroup);
    labelListsGroup = NULL;

    free(convertedArrays);
    convertedArrays = NULL;
}
void clean_converted_file(Converted_File *this){
    if (this==NULL)
        return;
    clean_convertion_phase(this->convertedAarrays, this->labelListsGroup);
    
    free(this);
    this=NULL;
    
}
void allocate_command_bit_array(int num_len, short **bit_array, int *bit_array_size, int *counter){
    
    if (num_len + *counter > *bit_array_size){
        *bit_array = (short*)realloc(*bit_array, sizeof(short) * (num_len + (*counter) + 10));
        (*bit_array_size)=num_len + (*counter) +10;
    }
 }
void add_to_command_array(short **command_bit_array,int *command_c,int  base){
    (*command_bit_array)[(*command_c)]=base;
    (*command_c)++;

}
/**
 * @brief translation the first memory word as instructed in the project notebook
 */
void bit_translator_first_word (const Line_Info *line_input, short **command_bit_array, int *command_array_length, int *command_c){
    short base=0;

        if (line_input->type==COMMAND) {
            if (line_input->targetP == NULL && line_input->sourceP == NULL){
                base = line_input->commandNum << 5;
                allocate_command_bit_array(1, command_bit_array, command_array_length, command_c);
                add_to_command_array(command_bit_array, command_c, base);
            }
            else {
                base = line_input->targetP->type << 2;
                if (line_input->sourceP != NULL)
                    base |= line_input->sourceP->type << 9;
                base |= line_input->commandNum << 5;

                allocate_command_bit_array(1, command_bit_array, command_array_length, command_c);
                add_to_command_array(command_bit_array, command_c, base);
            }
        }
        return;
}

/**
 * @brief the function that calls the semi function to convert the relevant data to its appropriate bit representation
 * it works as a decision tree- each type of command and param type shpuld be converted differantly- so we created semi
 * functions to do so for each senario
 */

void bit_translator_general(const Line_Info *line_input, Converted_Arrays *CA, Label_Lists_Group *LLG, short **command_bit_array,int *command_array_length, int *command_c){
    boolean both_param_reg= false;
    if (line_input->targetP==NULL && line_input->sourceP==NULL)
    return;
    if(line_input->sourceP!=NULL) {
        if (line_input->sourceP->type == REG && line_input->targetP!=NULL && line_input->targetP->type == REG){
            make_word_both_reg(line_input,command_bit_array,command_array_length, command_c );
            both_param_reg=true;
        }
        else if (line_input->sourceP->type == REG) 
            sourceP_reg_translation(line_input,command_bit_array,command_array_length, command_c);
        else if (line_input->sourceP->type == NUM) 
            sourceP_num_translation(line_input,command_bit_array,command_array_length, command_c);
        else if(line_input->sourceP->type==LABEL)
            label_param_into_SRLL(line_input->sourceP,CA, LLG);
    }
    if(line_input->targetP!=NULL){     
        if(line_input->targetP->type==REG && both_param_reg==false)
            targetP_reg_translation(line_input,command_bit_array,command_array_length, command_c);
        else if (line_input->targetP->type == NUM) 
            targetP_num_translation(line_input,command_bit_array,command_array_length, command_c);
        else if(line_input->targetP->type==LABEL)
            label_param_into_SRLL(line_input->targetP,CA, LLG);
    }
}
/**
 * @brief a funciton fotr the senario both params are type REG
 */
void make_word_both_reg(const Line_Info *line_input,short **command_bit_array,int *command_array_length, int *command_c){
    short base =0;
    base = *(line_input->sourceP->num )<< 7;
    base |= *(line_input->targetP->num) << 2;
    allocate_command_bit_array(1, command_bit_array, command_array_length, command_c);
    add_to_command_array(command_bit_array, command_c, base);
}
/**
 * @brief the translation for a source parameter type reg as instructed in the project notepad
 */
void sourceP_reg_translation(const Line_Info *line_input,short **command_bit_array,int *command_array_length, int *command_c ){
    short base =0;
    base = *(line_input->sourceP->num) << 7;
    allocate_command_bit_array(1, command_bit_array, command_array_length, command_c);
    add_to_command_array(command_bit_array, command_c, base);
} 
/**
 * @brief the translation for a source parameter type NUM, as instructed in the project notepad
 */
void sourceP_num_translation(const Line_Info *line_input,short **command_bit_array,int *command_array_length, int *command_c){
    short base=0;    
    base = *(line_input->sourceP->num)<<9;
    allocate_command_bit_array(1, command_bit_array, command_array_length, command_c);
    add_to_command_array(command_bit_array, command_c, base);
}
/**
 * @brief the translation for a source parameter type reg, as instructed in the project notepad
 */
void targetP_reg_translation(const Line_Info *line_input,short **command_bit_array,int *command_array_length, int *command_c){
    short base=0; 
    base = *(line_input->targetP->num) << 2;
    allocate_command_bit_array(1, command_bit_array, command_array_length, command_c);
    add_to_command_array(command_bit_array, command_c, base);
}
/**
 * @brief the translation for a target parameter type NUM, as instructed in the project notepad
 */
void targetP_num_translation(const Line_Info *line_input,short **command_bit_array,int *command_array_length, int *command_c){
    short base=0; 
    base = *(line_input->targetP->num)<<2;
    allocate_command_bit_array(1, command_bit_array, command_array_length, command_c);
    add_to_command_array(command_bit_array, command_c, base);    
}


