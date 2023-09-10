#ifndef GET_LINE_INFO_STRUCTS_H
#define GET_LINE_INFO_STRUCTS_H

#include "enums.h"


typedef struct Error Error;
typedef struct Command Command;
typedef struct Instruction Instruction;
typedef struct Param Param;
typedef struct Line_Info Line_Info;


struct Error{
    char *errorString;
    const char *input;
};
/**
 * @brief constructiong the command and instruction structs array - those arrays contain the language 
 * defenitions for each type of order specificaly;
 * those structs allow as to verify and now at the extracting line info which order can recieve 
 * what types (and how many) params
 *
 */
struct Command{
    const char *name;
    int num;
    Poss_P_Type SourcePT;
    Poss_P_Type TargetPT;
};

struct Instruction{
    const char *name;
    Order_Type type;

};
/**
 * @brief the struct that represent the params of a given order
 * as you can see - it has eather a number pointer (can be array if the order is .data/.string)
 * eather a label (label parameter)
 */
struct Param{
    Param_Type type;
    int *num;/*for( REG  and numeric types only*/
    int num_len;
    const char *label; /*only for labels*/

};
/**
 * @brief the struct that holds the information of the current line-
 * its parameters, type and the label of the line(if exists in line)
 * it has a pointer to the struct Param that represent the parameters
 *
 */
struct Line_Info{ /*כל המידע הרלוונטי אחריי פרונט אנד*/
    Order_Type type;/*entry/extern/data/string/command- אם זה אנטרי או אקסטרן- המידע היחידי שמתקבל זה הלייבל וכל השאר נול*/
    const char* label;/*אם אין תווית בשורה הנוכחית זה יאותחל לנול*/
    short commandNum;/*המספר שמייצג את 16 הפקודות האפשריות , רלוונטי רק עבור קומאנד*/
    const Param *sourceP;/* רלוונטי עבור אלו שיש להם אופרנד מקור- אחרת זה נול (אם זו הוראת דאטא או סטרינג, הפרמטר של השיטה יאותחל בפרמטר הזה)*/
    const Param *targetP;/*רלוונטי רק עבור קומאנד שיש להם אופרנד יעדת אחרת זה נול*/

};

#endif
