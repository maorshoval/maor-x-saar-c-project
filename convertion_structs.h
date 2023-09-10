#ifndef CONVERTION_STRUCTS_H
#define CONVERTION_STRUCTS_H
#include "enums.h"

typedef struct label_node label_node;
typedef struct Label_Trie_Node Label_Trie_Node;
typedef struct Label_List Label_List;
typedef struct Converted_Arrays Converted_Arrays;
typedef struct Label_Lists_Group Label_Lists_Group;
typedef struct Converted_File Converted_File;
/**
 * @brief a simple node that represent the label info- its string and 
 * index(the location of the label as where it was defined/used as a parameter)
 */
struct label_node {
    const char* label_name;
    Order_Type type; 
    int index;/*the location of the label in the file- to help convertiong the right adress*/
    struct label_node* next;
};
/**
 * @brief we used a trie struct to help as seach efficiantly labels- best time complexity for seach
 * the labels that are set to the trie are labels that were defined through the file.
 * each end of word holds a pointer to the labelnode it represents (at the node there is the relevant data of the label, as explained above)
 *  
 */
struct Label_Trie_Node {
    boolean isEndOfWord;
    label_node* label_data;
    struct Label_Trie_Node* children[ALPHABET_SIZE];
};

struct Label_List {
    label_node* head;
    label_node* tail;
};
/**
 * @brief a struct that holds toghter the converted arrays information-
 * the converted arrays themself, and their size and number of elements (the counters)
 */
struct Converted_Arrays {
    int command_c;/*counter*/
    int inst_c;/*counter*/
    short *command_bit_array;
    short *inst_bit_array;
    int inst_bit_array_size;
    int command_bit_array_size;
};
/**
 * @brief a structs that holds the relevant label lists
 */
struct Label_Lists_Group {
    Label_Trie_Node *labelTrieRoot;
    Label_List *trieList;
    Label_List *entryList;
    Label_List *second_run_labels_list;
    Label_List *externList;
};
/**
 * @brief the output file that holds all the relevant info that the last convertion needs to 
 * be able to produce the output files.
 */
struct Converted_File {
    Converted_Arrays *convertedAarrays;
    Label_Lists_Group *labelListsGroup;
};
#endif