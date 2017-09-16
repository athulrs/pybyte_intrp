#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code_init.h"

extern int obj_cnt;
int read_code(unsigned* pycbuf, struct code_obj* pobj, int cur, int size);
struct obj* init_obj(void)
{
    struct obj* pobj = malloc(sizeof *pobj);
    pobj->type = TYPE_INTEGER;
    return pobj;
}

void field_add(struct field* pycfield, int data)
{
    int length = pycfield->length;
    if (length < MAXVAL)
        pycfield->val[length++] = data;
    pycfield->length++;
}

int have_arg(unsigned opcode)
{
    if (opcode < HAVE_ARG)
        return FALSE;
    else
        return TRUE;
}

int find_start(unsigned* pycbuf, int start)
{
    for(; !(pycbuf[start] == TYPE_CODE && pycbuf[start+17] == TYPE_STRING); start++);
    return start+22;
}

int find_start_of_this_code(unsigned* pycbuf, int cur)
{
    while(pycbuf[cur] != TYPE_CODE && cur > 0)
        cur--;
    if (cur <= 0)
        return find_start(pycbuf, 0);
    return cur + 22;
}

int find_next_callable(unsigned* pycbuf, int start, int num)
{
    while(num--) 
    {
        start++;
        start = find_start(pycbuf, start);
    }
    return start;  
}

int callable(int cur, unsigned* pycbuf)
{
    if (pycbuf[cur] == LOAD_CONSTANT && pycbuf[cur+3] == MAKE_FUNCTION)
        return TRUE;
    else
        return FALSE;
}

int compute_offset(unsigned* pycbuf, int cur, int target)
{
    int start = find_start_of_this_code(pycbuf, cur);
    int end = start + target;
    int offset = 0;
    int opcode = pycbuf[cur];
    if (opcode == JUMP_FORWARD)
        end = cur + 3 + target;
    else
        cur = start;
    while(cur < end) 
    {
        if(have_arg(pycbuf[cur])) 
	{
            offset += 2;
            cur += 3;
        } 
	else 
	{
            offset += 1;
            cur += 1;
        }
    }
    return offset;
}

int length(unsigned* pycbuf, int cur) 
{
    int len = pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24; 
    return len ;
}

int get_op_arg(unsigned* pycbuf, int cur)
{
    int l = pycbuf[cur+1];
    int m = pycbuf[cur+2];
    int oparg = l | m << 8; 
    int opcode = pycbuf[cur];
    if(opcode >= JUMP_FORWARD && opcode <= POP_JUMP_IF_TRUE)
        oparg = compute_offset(pycbuf, cur, oparg);
    return oparg;
}

int get_code(unsigned* pycbuf, struct code_obj* pobj, int cur, int size)
{
    int func_idx = 0;
    int op_arg = 0;
    int num_obj = 1;
    int d = 0;
    int len = length(pycbuf, cur-5);
    int end = cur + len;
    while(cur < end-1 && !(cur >= size)) 
	{
        if(!(callable(cur, pycbuf))) 
	{
            field_add(&(pobj->code), pycbuf[cur]);
            if(have_arg(pycbuf[cur])) 
	    {
                op_arg = get_op_arg(pycbuf, cur);
                field_add(&(pobj->code), op_arg);
                cur += 3;
            } 
	    else 
	    {
                cur += 1; 
            }
        } 
	else 
	{
            d = find_next_callable(pycbuf, cur, num_obj);
            struct code_obj* pnew_obj = malloc(sizeof(*pnew_obj));
            func_idx = read_code(pycbuf, pnew_obj, d, size); 
            objects[func_idx]->val.pobj =  pnew_obj;
            objects[func_idx]->type =  TYPE_CODE;
            num_obj++;
            cur += 9; 
        }
    }
    field_add(&(pobj->code), pycbuf[cur]);
    return cur+1; 
}

int skip_element(unsigned* pycbuf, int cur)
{
    int len = length(pycbuf, cur);
    return cur + len + 5;
}

int find_end_of_code(unsigned* pycbuf, int cur)
{
    cur += 17; 
    cur = skip_element(pycbuf, cur); 
    int n_const = length(pycbuf, cur);
    cur += 5;
    while(n_const--) 
    {
        if (pycbuf[cur] == TYPE_INTEGER)
            cur += 5;
        else if (pycbuf[cur] == TYPE_NONE)
            cur += 1;
        else if (pycbuf[cur] == TYPE_CODE)
            cur = find_end_of_code(pycbuf, cur);
        else 
            cur++;
    }
    n_const = 0;
    while(1) 
    {
            if (pycbuf[cur] == TYPE_TUPLE)
                n_const++;
            if (n_const == 4)
                break;
            cur++; 
    }
    cur += 5;
    cur = skip_element(pycbuf, cur);
    cur = skip_element(pycbuf, cur);
    cur += 4;
    cur = skip_element(pycbuf, cur);
    return cur;
}

int get_consts(unsigned* pycbuf, struct code_obj* pobj, int cur, int size)
{
    int num_co = length(pycbuf, cur);
    int i = 0;
    cur += 5;
    while(i < num_co) 
    {
        if (pycbuf[cur] == TYPE_INTEGER) 
	{ 
            field_add(&(pobj->consts), length(pycbuf, cur));
            cur += 5;
        }
        else if(pycbuf[cur] == TYPE_NONE)
	{
            field_add(&(pobj->consts), 0);
            cur += 1;
        } 
	else if(pycbuf[cur] == TYPE_CODE) 
	{
            field_add(&(pobj->consts), 0);
            cur = find_end_of_code(pycbuf, cur);
        }
        i++;
    }
    return cur; 
}

void copy(char* s, unsigned* pycbuf, int cur, int length)
{
    while(length--) 
    {
        *s = pycbuf[cur++];
        s++;
    }
    *s = '\0';
}

int get_names(unsigned* pycbuf, struct code_obj* pobj, int cur)
{
    int n_names = length(pycbuf, cur);
    int func_idx = 0;
    struct obj* ptrobj;
    cur += 5;
    while(n_names--) 
    {
        if (pycbuf[cur] == TYPE_INTERN) 
	{
            ptrobj = malloc(sizeof *ptrobj);
            copy(ptrobj->name, pycbuf, cur+5, length(pycbuf, cur));
            objects[obj_cnt++] = ptrobj;             
            pobj->names[pobj->name_cnt++] = ptrobj; 
            cur = skip_element(pycbuf, cur);
        } 
	else if(pycbuf[cur] == TYPE_SREF) 
	{
            func_idx = length(pycbuf, cur);
            ptrobj = pobj->names[pobj->name_cnt++] = objects[func_idx];
            cur += 5;
        } 
	else 
            cur ++;
    }
    return cur;
}

int get_varnames(unsigned* pycbuf, struct code_obj* pobj, int cur)
{
    struct obj* ptrobj;
    int n_varnames = (pobj->varnames).length = length(pycbuf, cur);
    cur += 5;
    while(n_varnames--) {
        if (pycbuf[cur] == TYPE_INTERN) {
            ptrobj = init_obj();
            copy(ptrobj->name, pycbuf, cur+5, length(pycbuf, cur));
            objects[obj_cnt++] = ptrobj;
        
            cur = skip_element(pycbuf,  cur);
        } else if (pycbuf[cur] == TYPE_SREF) {
            cur += 5;
        } else
            cur++;
    }
    return cur;
}

int code_name(unsigned* pycbuf, struct code_obj* pobj, int cur)
{
    int n_field = 0;
    struct obj* ptrobj;
    while(1) {
            if (pycbuf[cur] == TYPE_TUPLE)
                n_field++;
            if (n_field == 2)
                break;
            cur++; 
    }
    cur += 5; 
    cur = skip_element(pycbuf, cur);
    if (pycbuf[cur] == TYPE_INTERN) 
    {
        ptrobj = init_obj();
        strncpy(ptrobj->name, (char *) &pycbuf[cur+5], length(pycbuf, cur)+1);
        objects[obj_cnt++] = ptrobj;
        pobj->name = ptrobj->name;
        return obj_cnt-1;
    }
    else 
        return length(pycbuf, cur);
}

int read_code(unsigned* pycbuf, struct code_obj* pobj, int cur, int size)
{
    cur = get_code(pycbuf, pobj, cur, size);
    cur = get_consts(pycbuf, pobj, cur, size);
    cur = get_names(pycbuf, pobj, cur);
    cur = get_varnames(pycbuf, pobj, cur);
    code_name(pycbuf, pobj, cur);
}
