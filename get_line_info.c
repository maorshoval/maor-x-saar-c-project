
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "enums.h"
#include "get_line_info_structs.h"
#include "get_line_info.h"
/*
those functions are part of the syntax error proccesing- the concentrate to a struct all the relevant data from the current line
if any problems accur- we add the erros to a struct and print the errors we found.
we may have a little bit overkilled here- we tried to cover as many syntax erros as possible- 
there are around 30 different errors check and printing accordingly, so this part contain many functions,
to cover many different senarios
*/
/**
 * @brief this function checks if a line is a comment or empty- if so-i skips it
 *
 */

boolean empty_or_comment_line(const char *input){
    const char *current=input;
    boolean result=false;
    current=skip(current,SPACES);
    if (*current==';')
        result=true;
    
    if (is_only_spaces(current, (size_t)strlen(current))==true)
        result=true;
    
    return result;
}

void check_line_length(char **input){
char *endOfLine;

    if ((*input)[MAX_LINE_LENGTH-2]!='\0' && (*input)[MAX_LINE_LENGTH-3]!='\n')
        printf("warning-line is too big and can cause missusage of data\n");
    /*eliminating the \n from the line so i could be sure there is no \n in the line string*/
    if ((endOfLine=strchr(*input,'\n'))!=NULL) 
        *endOfLine='\0';
}

boolean is_only_spaces(const char *current,size_t len){
    const char *start=current;
    if (current==NULL)
    return true;
    current=skip(current,SPACES);
    if ((*current == '\0' || isspace(*current)) && current<=(start+len))
        return true;
    else return false;
}
/**
 * @brief an important function that cheks the label propreties- accordingly with the given rules (only numbersand alphabets, must have colon.. ext.)
 *
 */
const char *label_deffinition_properties(const char *start, const char *end, Error *error, const Command *comm_array, const Instruction *inst_array,  Order_Type lineType){
    const char *label;
    char *alnum;
    if (start==end)
        return NULL;
    
    if ((alnum = strpbrk(start, ALNUM))!=NULL && alnum>=end){/*IN CASE BEGGINING IS LOOKING LIKE THIS: #//^mov @r3,@r4 or /\.string "asd" */
        add_error(error,"error- trying to assingn illegal characters between beginning of line and line order\n", start);
        return NULL;
    }
    label=create_label(start,(const char*)end);
    label_inner_propreties(start, label, error, comm_array, inst_array);

    if(*end !=':')
        add_error(error, "error- label is missing a colon at the end of its defenition\n", end);
    
    if (lineType==NOORDER)
        add_error(error, "error- trying to assign a label without an order after it \n", end);
    else if (lineType==ENTRY || lineType==EXTERN)
        add_error(error, "error- trying to assign an entry/extern instruction to a after label definition. label must have eather a data, a string, or a command order after its decleration in line\n", end);
    
    return label;
}

/**
 * @brief another semi function to check labels properties
 *
 */
boolean label_inner_propreties(const char *start, const char *label, Error *error, const Command *comm_array, const Instruction *inst_array){/*should use get label instead...*/
    boolean result= true;
    char *notALNUM;
    int i;
    
    if (!isalpha(*label)){
        add_error(error, "error-label does not start with an alphabetic character\n", start);
        result=false;
    }
    if ((notALNUM= strpbrk(label, SYMBOLS))!=NULL){
        add_error(error, "error- illegal characters in label defenition/label parameter- label must contains only alphabet and numeric characters\n", (start+(notALNUM-label)));
        result=false;
    }
    if (strlen(label)>31){
        add_error(error, "error- label is too long- label must be up to 31 characters \n", start+31);
        result=false;
    }

    for(i=0; i<16; i++)
        if(strcmp(comm_array[i].name, label)==0){
            add_error(error, "error- label name is a command name\n", start);
            result=false;
        }
    for(i=0; i<4; i++)
        if(strcmp(inst_array[i].name, label)==0){
            add_error(error, "error- label name is an instruction  name\n", start);
            result=false;
        }

    return result;
}

const char* create_label( const char *start, const char* end){
    size_t length;
    char *label;
    if (start == NULL || end == NULL ||start > end) {
        return NULL; 
    }
    length = end - start + 1;
    label = (char*)malloc(sizeof(char) * length);

    if (label != NULL) {
        strncpy(label, start, length - 1);
        label[length - 1] = '\0';
    }

    return label;
}
/**
 * @brief a function to decide which char is the closing char of a label- a fun name to make this worthwile while coding
 *
 */
const char* win_big(const char *firstLetter, const char *start_of_order){/*colon_vs_order_vs_space*/
    const char *first_space=strpbrk(firstLetter, SPACES);
    const char *colon = colon_vs_start(firstLetter);
    if (*colon==':')
        return colon;
    else if (first_space!=NULL &&first_space<start_of_order)
        return first_space;
    else return start_of_order;
}

const char *colon_vs_start(const char *firstLetter){
    const char *colon = strchr(firstLetter, COLON);
    if (colon!=NULL)
        return colon;
        else return firstLetter;
}
/**/
/**/
/**/
const char* check_start(const char *input, const char *current){
  current=skip(current,"\t\n\r\v\f: ");
    if (strchr(input, COLON)!=NULL && strchr(input, COLON)<current)
        printf("warning - illegal colon at start. if this line should be a comment- use ; instead\n");
    return current;
  }
  /**
 * @brief creates the error struct that allows us to collect erros along the way and print them properly at the end of the line proccesing
 *
 */
Error* set_error(const char *input){
    Error *error=(Error*)malloc(sizeof(Error));    
    error->errorString = (char*)calloc(1, sizeof(char));
    error->input=input;
    return error;
  }
  const char* get_pre_param_info(const char *input, const char **label, const Command *comm_array, const Instruction *inst_array, int *orderN, Order_Type *type, Error *error){
    const char *current=input;
    const char *start_of_order;
    const char *firstLetter;

    firstLetter= check_start(input, current);
    start_of_order= find_order(comm_array, inst_array, colon_vs_start(firstLetter), orderN, type);
    current=start_of_order;

    if (is_dot_before_inst(firstLetter, start_of_order, error, *type)==true)
        start_of_order--;

    if (!(type==NOORDER && *(colon_vs_start(firstLetter))!=COLON))
        *label= label_deffinition_properties(firstLetter, win_big(firstLetter, start_of_order) ,error, comm_array, inst_array, *type);

    current=check_between_order_and_param(current,comm_array, inst_array,*type,*orderN, error );
    return current;
}
/**
 * @brief the main function to extract the relevant data (info) of the current line. we created a strucct named line info 
 * that holds all the relevant data for a given line.
 * we used if else statment so it sored of works as a decision tree- each type of Order (command/instruction)
 *  gets a different semi-function to extract the relevlant data
 *
 */
const Line_Info* get_line_info(const char *input, int lineI, const Command *comm_array, const Instruction *inst_array, boolean *flag_errors_detected){
    const char *current=input ;
    const char *label=NULL;
    const char *labelParam=NULL;
    const Param *sourceParam=NULL;
    const Param *targetParam=NULL;
    const Line_Info *result=NULL;
    Error *error =set_error(input);
    Order_Type type;
    Param_Type paramType;
    int orderN;
    int *nums;
    int sizeN;

    current=get_pre_param_info(input, &label, comm_array, inst_array, &orderN, &type, error);

    if(type==NOORDER)
        add_error(error, "error- there is no proper order in the line\n", current);
    
    else if(type ==ENTRY ||type== EXTERN)
    {
        if(is_only_spaces(current, strlen(current))==true)
            add_error(error, "error- missing a label parameter after extern/entry instruction\n", current);
        else {
            current= label_param_propreties(&current, error,&paramType, comm_array, inst_array,&labelParam)+1;
            sourceParam=create_param(paramType,NULL,NULL,labelParam,0);
        }
    }

    else if (type==DATA || type==STRING)
    {
        nums=(int*)malloc(sizeof(int));
        sizeN=0;
        
        current= str_vs_data_param(type, &current, error,&nums, &sizeN); /*get_dataP(&current, error,&nums, &sizeN);  */
        sourceParam= create_param(NUM,NULL,nums,NULL,sizeN);
    }

    else if(type==COMMAND)
    {     
        sourceParam= get_comm_source_param(&current, comm_array, inst_array,error, orderN);
        current = check_param_overflow(current ,comm_array,orderN, error);
        targetParam=get_comm_target_param(&current, comm_array, inst_array,error, orderN);
    }

    if (type!=NOORDER)
        check_endP(current,error);
    
    if (is_line_syntax_legit(error, lineI, sourceParam, targetParam)==true)
        result =create_line_info(type, label,orderN, sourceParam, targetParam );
    else *(flag_errors_detected)=true;
    
    free(error->errorString);
    free(error);
    
    return result;
}
/**
 * @brief a function that allows extracting the data for a sorce Parameter of an order of the type command
 * it calls the semi function that check which type of parameter it is (reg/num/label)
 * if it found agood parameter- it sets it into the param struct 
 *
 */
const Param* get_comm_source_param(const char **currentPtr, const Command *comm_array, const Instruction *inst_array,Error *error, int orderN){
    const Param *sourceParam=NULL;
    
    if (comm_array[orderN].SourcePT!=T_NONE){
        if(strpbrk(*currentPtr,ALNUM)!=NULL){
            sourceParam=get_command_param(currentPtr, error, comm_array[orderN].SourcePT, comm_array, inst_array);              
            *currentPtr=check_comma(*currentPtr,error, true);
        }
        else add_error(error, "error- missing source param\n", *currentPtr);
    }
    return sourceParam;
}
const Param* get_comm_target_param(const char **currentPtr, const Command *comm_array, const Instruction *inst_array,Error *error, int orderN){
    const Param *targetParam=NULL;

    if (comm_array[orderN].TargetPT!=T_NONE){
        if (strpbrk(*currentPtr,ALNUM)!=NULL)
            targetParam=get_command_param(currentPtr, error, comm_array[orderN].TargetPT, comm_array, inst_array);
        else add_error(error, "error- missing a target paramm\n", *currentPtr);
    } 
    return targetParam;
}
/*a function that checks at the end of the run proccessing f any errors where detected.
if the error struct of the current line is empty- it returns true, esle- false*/
boolean is_line_syntax_legit(Error *error, int lineI, const  Param *sourceParam,const Param *targetParam){

    if(error->errorString[0]!='\0'){
        print_error(error, lineI);
        freeParam(sourceParam);
        freeParam(targetParam);
        return false;
        }
    else return true;
    }
    /**
 * @brief printing the error of the scurrent line- if any where detected
 *
 */
void print_error(Error *error, int lineI){
    int i;
    int lineI_len=0;
    const char *current=(error->input);
    printf(" At line %d:%s\n",lineI ,error->input);
    for (i=0;lineI>0;i++){
        lineI/=10;
        lineI_len++;
    }
    for (i=0;i<lineI_len+10;i++)
        printf(" ");
    for(i=0;i<strlen(error->input);i++){
        if (*current=='\t')
            printf("^\t");
        else printf("^");
        current++;
    }
    printf(" (line indexes)\n");
    printf("\n%s\n",error->errorString);

    for(i=0;i<80;i++)
        printf("-");
    printf("\n");
}
/**
 * @brief afunction to check f there are too many parameters for a given command order
 *
 */
const char* check_param_overflow(const char *current , const Command *comm_array, int orderN, Error *error){
    char *comma = strchr(current,COMMA);
    if(comm_array[orderN].SourcePT==T_NONE && comm_array[orderN].TargetPT!=T_NONE &&comma!=NULL &&is_only_spaces(comma+1, strlen(comma)==false)){
        add_error(error ,"error- too many params for this command type. causing conflicting translation\n",comma);
        return (const char*)(comma+1);
    }
    else return current;
}
const char* check_between_order_and_param(const char *current,const Command *comm_array, const Instruction *inst_array,Order_Type type, int orderN, Error *error ){
    if (type==NOORDER)
        return current;
    else if (type==COMMAND)
        current+=(strlen(comm_array[orderN].name)); 
    
    else current+=(strlen(inst_array[orderN].name));

    current=check_prior_param(current, orderN, error);
    return  current;
}
const Line_Info* create_line_info(const Order_Type type,const char* label, short orderN,const Param* sourceParam,const Param* targetParam ){
    Line_Info *thisLine=(Line_Info*)malloc(sizeof(Line_Info));
    thisLine->type=type;
    thisLine->label=label;
    thisLine->commandNum=orderN;
    thisLine-> sourceP=sourceParam;
    thisLine->targetP=targetParam;
    return thisLine;

}
void free_line(const Line_Info *thisLine){
    if (thisLine==NULL)
        return;
    if(thisLine->label!=NULL)
        free((void*)thisLine->label);
    if (thisLine-> sourceP!=NULL)
            freeParam(thisLine-> sourceP);
    if (thisLine->targetP!=NULL)
            freeParam(thisLine->targetP);

}

/**
 * @brief a very important function that extracts the data from the parameters of a command order- 
 * it calss the semi functions get source parameter 
 * and get target parameter
 *
 */
const Param* get_command_param(const char **currentPtr, Error *error, Poss_P_Type PPT,const Command *comm_array,const Instruction *inst_array){
    const char *labelP=NULL;
    int *regP=(int*)malloc(sizeof(int));
    int *numP=(int*)malloc(sizeof(int));
    Param_Type paramType=NONE;

    *currentPtr=check_regP(currentPtr, error, &paramType, regP);     

    if(PPT!=T_1_3_5 && PPT!=T_3_5 && paramType==REG)
        add_error(error,"illegal type of param- this command can not recieave a register as a param\n", *currentPtr);
    
    if (paramType!=REG)
    {
        free(regP);
        regP=NULL;

        *currentPtr=check_numP(currentPtr, error, &paramType, numP);
        if (PPT!=T_1_3_5 && paramType==NUM)
            add_error(error, "illegal type of param- this command can not recieave a number as a param\n", *currentPtr);
    }

    if (paramType!=REG && paramType!=NUM)
    {
        free(numP);
        numP=NULL;

        *currentPtr =label_param_propreties(currentPtr, error, &paramType, comm_array, inst_array, &labelP);
        if (PPT!=T_1_3_5 && PPT!=T_3_5 && PPT!=T_3)
            add_error(error,"illegal type of param- this command can not recieave a label as a param\n", *currentPtr);
    }   

    return create_param(paramType, regP, numP, labelP,1);
}
const Param* create_param(const Param_Type paramType, int *regP, int *numP, const char *labelP, int sizeN){
    Param *thisP=(Param*)malloc(sizeof(Param));
    thisP->type=paramType;
    
    switch (paramType) {
        case NONE:
            free(thisP);
            return NULL;
            break;

        case REG:
            thisP->num = (regP != NULL) ? regP : NULL;
            thisP->num_len=sizeN;
            thisP->label = NULL;
            break;

        case NUM:
            thisP->num = (numP != NULL) ? numP : NULL;
            thisP->num_len=sizeN;
            thisP->label = NULL;
            break;

        case LABEL:
            thisP->num = NULL;
            thisP->label = (labelP != NULL) ? labelP : NULL;
            break;
    }
    return (const Param*)thisP;
}
/**
 * @brief the function that checks if a parameter is register- if it is a register with error-
 *  it add the errors to the error struct- i
 *if it is valid- it set the number that represent the reg num and set the type of the param to reg
 *
 */
const char* check_regP(const char **currentPtr, Error *error, Param_Type *paramType, int *regP){
    char *regop= strstr(*currentPtr, REGISTER);
    *currentPtr=skip(*currentPtr,SPACES);
    if (regop==*currentPtr){
        if (strlen(regop)>2 && isdigit(*(regop+2))){
            *paramType=REG;
            *regP=(int)strtol((regop+2), (char**)currentPtr, 10);
        }
        if(strlen(regop)>=2 && !isdigit(*(regop+2))){
            add_error(error, "missing register number\n", *currentPtr);
            *paramType=REG; 
            *regP= -1;
            (*currentPtr)+=2;
        }
        if (*regP>7){
        add_error(error,"illegal register number\n", *currentPtr);
        *paramType=REG; 
        *regP= -1;
        }
    }
    return *currentPtr;
}
const char* check_prior_param(const char *current, int orderN, Error *error){
    if (!isspace(*current)&& strpbrk(current, ALNUM)!=NULL && orderN<14)      
        add_error(error, "error- there is no space between Order and param\n", current);
        current=check_comma(current, error ,false);
    
    return current;
}
/**
 * @brief a function that checks the label parameter propreties- uses one of the function that checks the inner 
 * propreties of a label - to see that all syntax rules apply
 *
 */
const char *label_param_propreties(const char **currentPtr, Error *error, Param_Type *paramType, const Command *comm_array, const Instruction *inst_array, const char **labelP){
    const char *start= *currentPtr;
    const char *label;
    const char *end= strpbrk(start,", \t");
    if (end==NULL)
        end=strchr(start,'\0');
    label =create_label(start, end);
    label_inner_propreties(start, label, error, comm_array, inst_array);
    *labelP=label;
    *paramType= LABEL;
    
    return (const char*)end;
}
/**
 * @brief a function that extract the string parameter of the .string instruction
 *
 */
const char* get_strP(const char **currentPtr, Error *error, int **nums, int * sizeN){
    const char *start;
    char *end;
    boolean flag_empty=false;

    start=skip(*currentPtr,SPACES);

    if (*start!='\"') 
        add_error(error,"parameter is missing an opening quotation mark\n", start);
        else start++;
    
    if (*start=='\0'){
        add_error(error,"string instruction is missing parameters\n", start);
        flag_empty=true; 
    }
    
    end=strchr(start,'\"');
    if (end==NULL){
        add_error(error,"parameter is missing an ending quotation mark\n", start);
        end= (strchr(start,'\0')-1);
    }

    if (end==start){
        add_error(error, "error- trying to assign an empty string- causing conflictiong translations\n", start);
        flag_empty=true;
    }

    if (flag_empty==false)
        *nums=create_strP(start,end,sizeN);
    
    return (const char*)(end+1);
}

const char* str_vs_data_param(Order_Type type, const char **currentPtr, Error *error, int **nums, int *sizeN){
    if (type==DATA)
        return get_dataP(currentPtr, error,nums, sizeN);
    else if (type==STRING)
        return get_strP(currentPtr, error,nums, sizeN);
        
        else return NULL;/*DEBUGGING HANDELING*/
}

int* create_strP( const char *start, const char *end, int *sizeN){
    int i=0;
    int *nums;
    *sizeN=(int)(end-start+1);
    nums=(int*)malloc(sizeof(int)*(*sizeN));
    while (start<end){
        nums[i]=(int)(*start);
        i++;
        start++;
    }
    nums[i]=0;

    return nums;
}
/**
 * @brief the function that adds error to the erro struct 
 *
 */
void add_error(Error *error, char *add, const char *current) {
    size_t len1 = strlen(error->errorString);
    size_t len2 = strlen(add);
    size_t index;
    size_t newLength;
    char addPre[30];
    
    if(current==NULL)
        current=error->input;
    
    index = current - error->input;
    sprintf(addPre, "Error - at index %.*d: ", 2, (int)index);
    
    
    newLength = len1 + strlen(addPre) + len2 + 1; 
    
    error->errorString = (char *)realloc(error->errorString, newLength);

    strcat(error->errorString, addPre);
    strcat(error->errorString, add);
}


const char* skip(const char *current, const char *generic) {
    char *match;
    boolean flag_stop=false;

    while (flag_stop==false && current!=NULL && *current != '\0') {
        match = strpbrk(current, generic);

        if (match == current) 
            current++;
            else flag_stop=true; 
    }
return current;
}
/**
 * @brief a function to verify there is a dot before the instruction orders
 *
 */
boolean is_dot_before_inst(const char *input, const char *curr_order, Error *error, Order_Type type){
    if (type==COMMAND||type==NOORDER)
        return false;
    if (input==curr_order || (curr_order>input && *(curr_order-1)!='.')){
        add_error(error, "error- missing a dot before data instruction\n", input);
        return false;
    }
    return true;
}
/**
 * @brief a function to extract the .data instruction  paramters (numbers) 
 *
 */
const char* get_dataP(const char **currentPtr, Error *error, int ** nums, int *sizeN){
    const char *next=*currentPtr;
    const char *prev=*currentPtr;
    boolean flagEnd=false;
    int i=0;
    Param_Type paramType=NONE;
    if(is_only_spaces(*currentPtr, strlen(*currentPtr))==true){
        add_error(error,"error- data instruction is missing parameters\n", *currentPtr);
        flagEnd=true;
    } 
    while (*next!='\0'&& flagEnd==false){
        paramType=NONE;   
        next=skip(next, SPACES); 
        if(strpbrk(next ,DIGITS)==NULL)
            flagEnd=true;
        
        if (flagEnd==false){
            next =skip(next, "\t ,");/*skipping all spaces and comms (helps avoiding double error types for consecutive commas)*/
            
            next = inner_get_dataP(&next,&prev, &i, error, nums, sizeN, &paramType);
            
            if (paramType==NONE)
                next=skip(next,NON_NUMERIC);
        }
    }
    return next;
}
const char* inner_get_dataP(const char **next, const char **prev,  int *i, Error *error, int ** nums, int *sizeN, Param_Type *paramType){
    int numP;
    *next=check_numP(next, error, paramType, &numP);

    if (*paramType==NUM){
        if (*i>=1){
            check_comma(*prev, error, true);
            *nums =(int*)realloc(*nums, sizeof(int)*(*(sizeN)+1)); 
        }
        (*nums)[*sizeN]=numP;
        (*sizeN)++; 
        *prev=*next;
        (*i)++;
    }
    else add_error(error, "error- illegal .data param \n", *next);

    return *next;
}
/**
 * @brief a function that check if a command order param is type num
 * if so- it set the number as the param and the param type as NUM
 *
 */

const char* check_numP(const char **currentPtr, Error *error, Param_Type *paramType, int *numP){
    const char *temp=*currentPtr;
    double num;
    
    temp=skip(temp,SPACES);
    num=strtod(temp, (char**)currentPtr);
    if (num>(int)num)
        add_error(error,"illegal implementation of a numeric parameter- the number should be an integer (double and floats are illegal)\n", *currentPtr);
    
    if (num <-512 || num>511)
        add_error(error,"number is too big- number should be an integer (msut be between -512 and 511)\n", *currentPtr);
    
    if (temp<*currentPtr){
        *paramType=NUM;
        *numP=(int)num;
    }

    return *currentPtr;
}
/**
 * @brief checks the ends of the line- to see that there are no extra chars after the last param
 *
 */
boolean check_endP(const char *current, Error *error){
    if (current==NULL)
        return true;   
    current= check_comma(current, error, false);
    is_only_spaces(current, (size_t)strlen(current));
    if (is_only_spaces(current, (size_t)strlen(current))==false){
        add_error(error, "excessive character at the end of the line\n", current );
        return false;
    }
    return true;
}

const char* check_comma(const char *current, Error *error, boolean shouldBe){
    current=skip(current, SPACES);
    if (*current==COMMA && shouldBe==true){
        current++;
        current=skip(current, SPACES);
        
        if(*current==COMMA){
            add_error(error, "error- consecutive commas. there should be only one comma between params\n", current);
            current++;
        }
    }
    else if (*current!=COMMA && shouldBe==true)
        add_error(error, "missing a comma between params\n", current);    

    else if (*current==COMMA && shouldBe==false){
        add_error(error, "illegal comma location- comma should be used only between parameters\n", current);
        current++;
    }
    return current;
}
/**
 * @brief important function that checks if there is an order in the line, 
 * and not only that- it finds the first accuring order that is not part of a label
 * for example- a label can be movw2:
 * which contain a command name- put totally legitamate, so it set the first order 
 * to the left that is not a part of the label/parameter
 * in this way we avoidtrying to read a label or a parameter as the order of the line.
 *
 */
const char* find_order(const Command *comm_array,const Instruction *inst_array, const char *current, int *this_order, Order_Type *type ){
    char *curr_order;
    char *first_order=(char*)current + (strlen(current)-1);
    int i;
    boolean flag_found=false;

    for(i=0; i<16; i++){
        curr_order = strstr(current, comm_array[i].name);
        if (curr_order!=NULL && curr_order<first_order && is_legit(current, curr_order)==true){/*making sure we get the left most order in the string*/
            flag_found=true;
            first_order=curr_order;
            *this_order=i;
            *type=COMMAND;
        }
    }
    for (i=0; i<4; i++){
        curr_order = strstr(current, inst_array[i].name);
        if (curr_order!=NULL && curr_order<first_order && is_legit(current, curr_order)==true){/*making sure we get the left most order in the string*/
            flag_found=true;
            first_order=curr_order;
            *this_order=i;
            *type=inst_array[i].type;
        }
    }
    if (flag_found==true)  
        return (const char*)first_order;
    
    else *type=NOORDER;
    return current;
}
boolean is_legit(const char *current,const  char *curr_order){
    if (current==curr_order)
        return true;
    else if (curr_order>current && !isalnum(*(curr_order-1))){
        return true;
    }
        else return false;
}

void freeParam(const Param *thisP){
    if(thisP==NULL)
        return;
    if (thisP->label!=NULL)
        free((void*)(thisP->label));
    if (thisP->num!=NULL)
        free(thisP->num);

    free((void*)thisP);
}
/**
 * @brief constructiong the command and instruction structs array - those arrays contain the language 
 * defenitions for each type of order specificaly;
 * those structs allow as to verify and now at the extracting line info which order can recieve 
 * what types (and how many) params
 *
 */
const Command* set_comm_array(){
    Command *comm_array= (Command*)malloc(sizeof(Command)*16);

    comm_array[0].name = "mov";
    comm_array[0].num = 0;
    comm_array[0].SourcePT=T_1_3_5;
    comm_array[0].TargetPT=T_3_5;

    comm_array[1].name = "cmp";
    comm_array[1].num = 1;
    comm_array[1].SourcePT=T_1_3_5;
    comm_array[1].TargetPT=T_1_3_5;

    comm_array[2].name = "add";
    comm_array[2].num = 2;
    comm_array[2].SourcePT=T_1_3_5;
    comm_array[2].TargetPT=T_3_5;

    comm_array[3].name = "sub";
    comm_array[3].num = 3;
    comm_array[3].SourcePT=T_1_3_5;
    comm_array[3].TargetPT=T_3_5;

    comm_array[4].name = "not";
    comm_array[4].num = 4;
    comm_array[4].SourcePT=T_NONE;
    comm_array[4].TargetPT=T_3_5;

    comm_array[5].name = "clr";
    comm_array[5].num = 5;
    comm_array[5].SourcePT=T_NONE;
    comm_array[5].TargetPT=T_3_5;

    comm_array[6].name = "lea";
    comm_array[6].num = 6;
    comm_array[6].SourcePT=T_3;
    comm_array[6].TargetPT=T_3_5;

    comm_array[7].name = "inc";
    comm_array[7].num = 7;
    comm_array[7].SourcePT=T_NONE;
    comm_array[7].TargetPT=T_3_5;

    comm_array[8].name = "dec";
    comm_array[8].num = 8;
    comm_array[8].SourcePT=T_NONE;
    comm_array[8].TargetPT=T_3_5;

    comm_array[9].name = "jmp";
    comm_array[9].num = 9;
    comm_array[9].SourcePT=T_NONE;
    comm_array[9].TargetPT=T_3_5;

    comm_array[10].name = "bne";
    comm_array[10].num = 10;
    comm_array[10].SourcePT=T_NONE;
    comm_array[10].TargetPT=T_3_5;

    comm_array[11].name = "red";
    comm_array[11].num = 11;
    comm_array[11].SourcePT=T_NONE;
    comm_array[11].TargetPT=T_3_5;

    comm_array[12].name = "prn";
    comm_array[12].num = 12;
    comm_array[12].SourcePT=T_NONE;
    comm_array[12].TargetPT=T_1_3_5;

    comm_array[13].name = "jsr";
    comm_array[13].num = 13;
    comm_array[13].SourcePT=T_NONE;
    comm_array[13].TargetPT=T_3_5;

    comm_array[14].name = "rts";
    comm_array[14].num = 14;
    comm_array[14].SourcePT=T_NONE;
    comm_array[14].TargetPT=T_NONE;

    comm_array[15].name = "stop";
    comm_array[15].num = 15;
    comm_array[15].SourcePT=T_NONE;
    comm_array[15].TargetPT=T_NONE;

    return comm_array;

}
const Instruction* set_inst_array(){
    Instruction *inst_array=(Instruction*)malloc(sizeof(Instruction)*4);

    inst_array[0].name = "data";
    inst_array[0].type = DATA;

    inst_array[1].name = "string";
    inst_array[1].type = STRING;
    
    inst_array[2].name = "entry";
    inst_array[2].type = ENTRY;
    
    inst_array[3].name = "extern";
    inst_array[3].type = EXTERN;

    return inst_array;

}
