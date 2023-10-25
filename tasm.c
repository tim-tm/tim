#include "tasm.h"

#define MAX_PROGRAM_SIZE 1024

void push_program(Inst program[], int *program_size, Inst value){
    assert(*program_size < MAX_PROGRAM_SIZE && "Program size cannot exceed max program size\n");
    program[(*program_size)++] = value;
}

Inst pop_program(Inst program[], int program_size){
    assert(program_size > 0 && "STACK UNDERFLOW\n");
    return program[--program_size];
}

Inst *generate_instructions(ParseList *head, int *program_size){
    Inst *program = malloc(sizeof(Inst));
    Inst_Set insts[INST_COUNT + 1] = {    
        INST_NOP, INST_PUSH, INST_POP, INST_DUP, INST_INDUP, INST_SWAP, INST_INSWAP, INST_ADD, INST_SUB, INST_MUL, 
        INST_DIV, INST_MOD, INST_ADD_F, INST_SUB_F, INST_MUL_F, INST_DIV_F, INST_MOD_F, INST_CMPE, INST_CMPNE, INST_CMPG, 
        INST_CMPL, INST_CMPGE, INST_CMPLE, INST_JMP, INST_ZJMP, INST_NZJMP, INST_PRINT, INST_HALT, INST_COUNT};

    while(head != NULL){
        assert(head->value.type != TYPE_NONE && "Value should not be none\n");
        assert(head->value.type < (TokenType)INST_COUNT);

        Inst *instruction = malloc(sizeof(Inst));
        instruction->type = insts[head->value.type];
        if(head->value.type == TYPE_INDUP || head->value.type == TYPE_INSWAP || head->value.type == TYPE_JMP || head->value.type == TYPE_ZJMP || head->value.type == TYPE_NZJMP){
            head = head->next;
            instruction->value.as_int = atoi(head->value.text);
        }
        if(head->value.type == TYPE_PUSH){
            head = head->next;
            if(head->value.type == TYPE_INT){
                instruction->value.as_int = atoi(head->value.text);
            } else if(head->value.type == TYPE_FLOAT){
                instruction->value.as_float = atof(head->value.text);
            } else if(head->value.type == TYPE_CHAR){
                instruction->value.as_char = head->value.text[0];
            } else if(head->value.type == TYPE_STRING){
                int i = 0;
                while(head->value.text[i] != '\0'){
                    instruction = malloc(sizeof(Inst));
                    instruction->type = insts[TYPE_PUSH];
                    instruction->value.as_char = head->value.text[i];
                    printf("typoe is %c\n", head->value.text[i]);
                    push_program(program, program_size, *instruction);
                    i++;
                }
                    printf("broke out is %c\n", head->value.text[i]);
                instruction->type = insts[TYPE_PUSH];
                instruction->value.as_char = '\0';
                //instruction->value.as_pointer = head->value.text[0];
            } else {
                assert(false && "you should not be here\n");
            }
        }
        push_program(program, program_size, *instruction);
        head = head->next;
    }
    return program;
}

char *chop_file_by_dot(char *file_name){
    int index;
    char *result = malloc(sizeof(char) * 64);
    for(index = 0; file_name[index] != '.' && file_name[index] != '\0'; index++){
        result[index] = file_name[index];
    }
    snprintf(result + index, 5, ".tim");
    return result;
}

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Usage: %s <file_name.tasm>\n", argv[0]);
        exit(1);
    }

    char *file_name = argv[1];
    char *output_file = chop_file_by_dot(file_name);
    Lexer lex = lexer(file_name);
    ParseList list = parser(lex);
    //print_list(&list);
    int program_size = 0;
    Inst *program = generate_instructions(&list, &program_size);
    Machine machine = {.instructions = program, .program_size = program_size};
    write_program_to_file(&machine, output_file);
    return 0;
}
