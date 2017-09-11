#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "code_init.h"

int obj_cnt;
int main(int argc,char **argv)
{ 
    FILE *ptr;
    int flag = 1;
    char* pycfile = argv[1];
    unsigned pycbuf[MAXPYC] = {0};
    unsigned char buf[100];
    memset(buf,0,sizeof(buf));
    int start,size,i=0,j=0;
    struct code_obj* code_obj = malloc(sizeof *code_obj);
    ptr = fopen(argv[1],"rb");
    while(flag)
    {
	if(fread(buf,1,4,ptr) != 4) 
	{
        flag = 0;
	break;
    	}
    	for(i=0;i<4;i++)
    	{
	   pycbuf[j++] = buf[i];
    	}
    }
    size = j;
    while(!(pycbuf[start]==63 && pycbuf[start+17]==73))
    {
	start++;
    }
    start+=22;
    read_code(pycbuf,code_obj,start,size); 
    execute(code_obj);    
    return 0;
}


void execute(struct code_obj* pobj) 
{
    int count = 0;
    int opcode = 0;
    struct code_obj* pfun;
    while(count < (pobj->code).length)
    {
	opcode = get_element(&(pobj->code), count);
        switch(opcode) 
	{
            case LOAD_CONSTANT: 
                load_const(pobj, count);
                count += 2;
                break;

            case LOAD_NAME: 
                load_name(pobj, count);
                count += 2;
                break;

            case LOAD_GLOBAL: 
                load_global(pobj, count);
                count += 2;
                break;

            case STORE_NAME:
                store_name(pobj, count);
                count += 2;
                break;
                
            case PRINT_ITEM:
                print_item();
                count += 1;
                break;
            
            case PRINT_NEWLINE:
                print_line();
                count += 1;
                break;

            case COMPARE_OP:
                compare_op(pobj, count);
                count += 2;
                break;
                
            case BINARY_ADD:
                binary_add();
                count += 1;
                break;

            case BINARY_MULTIPLY:
                binary_multiply();
                count += 1;
                break;

            case BINARY_DIVIDE:
                binary_divide();
                count += 1;
                break;

            case BINARY_SUBTRACT:
                binary_subtract();
                count += 1;
                break;

            case BINARY_MODULO:
                binary_modulo();
                count += 1;
                break;
            
            case POP_JUMP_IF_FALSE:
                count = pop_jump_if_false(pobj, count);
                break;
            
            case POP_JUMP_IF_TRUE:
                count = pop_jump_if_true(pobj, count);
                break;
            
            case JUMP_FORWARD:
                count = jump_forward(pobj, count);
                break;
            
            case JUMP_ABSOLUTE:
                count = jump_absolute(pobj, count);
                break;

            case SETUP_LOOP:
                count += 2;
                break;

            case CALL_FUNCTION:
                call_function(pobj, count);
                count += 2;
                break;

            case LOAD_FAST:
                load_fast(pobj, count);
                count += 2;
                break;
            
            case STORE_FAST:
                store_fast(pobj, count);
                count += 2;
                break;
            
            case POP_TOP:
                pop_top();
                count += 1;
                break;

            case RETURN_VALUE:
                return;

            default:
                return;
        }
    }
}
