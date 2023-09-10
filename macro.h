#ifndef MACRO_H
#define MACRO_H
#include "enums.h"

typedef struct macro_node macro_node;
typedef struct MacroTrieNode MacroTrieNode;

struct macro_node {
    char* macro_name;
    char* macro_content;
    macro_node* next;
};

struct MacroTrieNode {
    boolean isEndOfWord;
    macro_node* macro_data;
    MacroTrieNode* children[ALPHABET_SIZE];
};


MacroTrieNode* createMacroNode();

void insert_macro(MacroTrieNode* root, const char* key, macro_node* macro);

macro_node* search_macro(MacroTrieNode* root, const char* key);

void freeMacroTrieNode(MacroTrieNode* node);

void freeMacroTrie(MacroTrieNode* root);

void macro_expand (char *input_array_str);

void print_beginnig();
#endif
