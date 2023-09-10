
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "enums.h"
#include "macro.h"
#include "get_line_info.h"

/**
 * @brief expands the macros throgh a trie structure ,
 * saves macro name and a pointer to the content while initialized inside the trie
 * checks if the name of the macro is in the trie
 * and if it does it prints the content of the macro in the new am file
 * outputs a file after macro expansion
 * @param input_array_str an array of the file names that need to be open
 */
void macro_expand (char *input_array_str){
    FILE *input_file;
    FILE *output_file;
    MacroTrieNode* root = createMacroNode();
    char filename_with_extension_as[MAX_LINE_LENGTH];
    char filename_with_extension_am[MAX_LINE_LENGTH];

    macro_node* current_macro;
    macro_node* new_macro;
    macro_node* print_content;

    char line[MAX_LINE_LENGTH];
    
    strcpy(filename_with_extension_as, input_array_str);
    strcat(filename_with_extension_as, ".as");

    strcpy(filename_with_extension_am, input_array_str);
    strcat(filename_with_extension_am, ".am");

    input_file = fopen(filename_with_extension_as, "r");
    output_file = fopen(filename_with_extension_am, "w");

    current_macro = NULL;
    new_macro = NULL;
    print_content = NULL;


    if (input_file == NULL) {
        perror("Failed to open input file");
        remove(filename_with_extension_am);
        freeMacroTrie(root);
        return;
    }

    if (output_file == NULL) {
        perror("Failed to open output file");
        return;
    }

    while (fgets(line, sizeof(line), input_file) != NULL) {
        char* macro_content;
        int i=0;
        while(line[i]!= '\0' && isspace(line[i]) && i<MAX_LINE_LENGTH) {
        i++;
        }
        if(line[i]=='\0')
            continue; /* Move to  the next iteration because the line is empty */
    
        if (strncmp(line+i, "mcro", 4) == 0) {
            /* Extract the macro name */
            const char* macro_name_check = line + 5; /* Skip "mcro" */
            char* newline = strchr(macro_name_check, '\r');
            if (newline) {
                *newline = '\0';
            }

            new_macro = (macro_node*)calloc(1, sizeof(macro_node));
            new_macro->macro_name = (char*)malloc(strlen(macro_name_check) + 1);
            strcpy(new_macro->macro_name, macro_name_check);

            new_macro->macro_content = NULL;

            new_macro->next = NULL;

            insert_macro(root, macro_name_check, new_macro);
            if (current_macro == NULL) {
                current_macro = new_macro;

            }

            if (current_macro && current_macro!=new_macro) {
                current_macro->next = new_macro;
            }

            current_macro = new_macro;

            /* Start collecting macro content */
            macro_content = (char*)malloc(MAX_LINE_LENGTH);
            macro_content[0] = '\0';
            while (fgets(line, sizeof(line), input_file) != NULL) {
                 i=0;
                 while(line[i]!= '\0' && isspace(line[i]) && i<MAX_LINE_LENGTH) {
                    i++;
                }
                if(line[i]=='\0')
                    continue; /* Move to  the next iteration because the line is empty */
                
                if (strncmp(line+i, "endmcro", 7) == 0) {
                    break; /* End of macro context */
                }
                strcat(macro_content, line);
            }

            current_macro->macro_content = macro_content;

        } else {
            line[strcspn(line, "\r\n")] = '\0';
            print_content = search_macro(root, line);
            if (print_content != NULL) {
                fprintf(output_file, "%s", print_content->macro_content);
            } else {
                fprintf(output_file, "%s\n", line);
            }
        }
    }

    /* Free the Macro Trie memory */
    freeMacroTrie(root);

    /* Free the linked list 
    current = head_macro;
    while (current != NULL) {
        macro_node* next = current->next;
        free(current->macro_name);
        free(current->macro_content);
        free(current);
        current = next;
    }*/

    fclose(input_file);
    fclose(output_file);
}
/**
 * @brief Creates and initializes a MacroTrieNode object.
 * 
 * This function dynamically allocates memory for a MacroTrieNode object and initializes its properties.
  This function is used to create a new node for constructing a macro trie structure.
 * 
 * @return A pointer to the newly created MacroTrieNode, or NULL if memory allocation fails.
 */
MacroTrieNode* createMacroNode() {
    MacroTrieNode* node = (MacroTrieNode*)malloc(sizeof(MacroTrieNode));
    if (node) {
        int i;
        node->isEndOfWord = false;
        node->macro_data = NULL;
        for (i = 0; i < ALPHABET_SIZE; i++) {
            node->children[i] = NULL;
        }
    }
    return node;
}

/**
 * @brief Inserts a macro node into a macro trie structure.
 * 
 * This function inserts a macro node into a trie data structure using characters from the provided key.
 * The macro node is associated with the given key and is stored at the leaf node corresponding to the key's characters.
 * 
 * @param root Pointer to the root node of the macro trie.
 * @param key The key string used to determine the insertion path in the trie.
 * @param macro Pointer to the macro node to be inserted and associated with the key.
 */
void insert_macro(MacroTrieNode* root, const char* key, macro_node* macro) {
    int i;
    int index;
    MacroTrieNode* curr = root;
    for (i = 0; key[i] != '\0'; i++) {
        index = (key[i] >= 'a' && key[i] <= 'z') ? key[i] - 'a' :
                (key[i] >= 'A' && key[i] <= 'Z') ? key[i] - 'A' :
                (key[i] >= '0' && key[i] <= '9') ? key[i] - '0' + 26 : -1;
        if (index != -1 && curr->children[index] == NULL) {
            curr->children[index] = createMacroNode();
        }
        if (index == -1) {
            continue; /* Skip non-alphanumeric characters */
        }
        curr = curr->children[index];
    }
    curr->isEndOfWord = true;
    curr->macro_data = macro;
}



/**
 * @brief Searches for a macro node associated with the given key in a macro trie structure.
 * 
 * This function searches for a macro node associated with the provided key in a trie data structure.
 * It traverses the trie based on the characters in the key and returns the macro node associated with
 * the key if found in this node the macro content will be found.
 * 
 * @param root Pointer to the root node of the macro trie.
 * @param key The key string to search for in the trie.
 * @return A pointer to the macro node associated with the key, or NULL if the key is not found in the trie.
 */
macro_node* search_macro(MacroTrieNode* root, const char* key) {
    MacroTrieNode* curr = root;
    int i;
    int index;
    for (i = 0; key[i] != '\0'; i++) {
        if (key[i] == '\r' || key[i] == '\n') {
            break;
        }
        index = (key[i] >= 'a' && key[i] <= 'z') ? key[i] - 'a' :
                (key[i] >= 'A' && key[i] <= 'Z') ? key[i] - 'A' :
                (key[i] >= '0' && key[i] <= '9') ? key[i] - '0' + 26 : -1;
        if (index == -1 || curr->children[index] == NULL) {
            return NULL;
        }
        curr = curr->children[index];
    }
    if (curr != NULL && curr->isEndOfWord==true)
        return curr->macro_data;
    else
        return NULL;
}
/**
 * @brief Recursively frees memory associated with a MacroTrieNode and its children.
 * 
 * This function traverses a macro trie starting from the provided node and recursively
 * frees memory for the node and its child nodes, including associated macro content.
 * 
 * @param node Pointer to the macro trie node to be freed.
 */
void freeMacroTrieNode(MacroTrieNode* node) {/*chamged function*/
    int i;
    if (node == NULL) {
        return;
    }
    
    for (i = 0; i < ALPHABET_SIZE; i++) {
        freeMacroTrieNode(node->children[i]);
    }
    if (node->macro_data != NULL) {
        free(node->macro_data->macro_name);
        free(node->macro_data->macro_content);
        free(node->macro_data);
    }
    free(node);
}

/**
 * @brief Frees memory associated with a macro trie.
 * 
 * This function initiates the process of freeing memory for a macro trie by calling
 * the recursive `freeMacroTrieNode` function with the root node of the trie.
 * 
 * @param root Pointer to the root node of the macro trie.
 */
void freeMacroTrie(MacroTrieNode* root) {
    freeMacroTrieNode(root);
}
void print_beginnig(){
    printf("Hello, this is an assembler translator program.\nthis program reads files and translate relevant orders (command orders or instruction orders) to base 64.\n");
    printf("Authors: Saar Nurflus, Maor Shoval.");
}