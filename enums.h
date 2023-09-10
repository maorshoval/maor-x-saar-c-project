
#ifndef ENUMS_H
#define ENUMS_H

#define REGISTER "@r"
#define COLON ':'
#define COMMA ','
#define DIGITS "1234567890"
#define SYMBOLS "!\"'#$%&'()*+-./:;<=>?@[\\]^_`{|}~" /*excluding comma*/
#define SPACES " \t\n\r\v\f\n"
#define ALPHABET "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define COMBINE(x, y) x  y
#define NON_NUMERIC COMBINE(COMBINE(SYMBOLS, COMBINE(SPACES, ALPHABET)), "")
#define ALNUM "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890"
#define ALPHABET_SIZE 62
#define FIRST_MEMORY_CELL 100
#define MAX_LINE_LENGTH 81
#define MAX_FILE_NAME 50

typedef enum {
    false=0,
    true=1
} boolean;
/**
 * @brief the param type enum- allows us to know what type of parameter is the current parameter
 */
typedef enum Param_Type{
    NONE,
    NUM=1,
    LABEL=3,
    REG=5
}Param_Type;
/**
 * @brief the type of the current order, as explained in the language defenition.
 * command repesent the 16 command order (mov till stop)
 * under instruction orders- data/string/entry/extern
 */
typedef enum Order_Type{
    NOORDER,
    ENTRY,
    EXTERN,
    DATA,
    STRING,
    COMMAND
}Order_Type;
/**
 * @brief an enum that reprenebt the sfecific params a command order can recieve correctly
 */
typedef enum Poss_P_Type{
    T_NONE,/*gets no parameters*/
    T_3,/*gets only type label*/
    T_3_5,/*gets label and reg*/
    T_1_3_5/*hets all*/

}Poss_P_Type;

#endif