#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code_init.h"

extern int obj_cnt;
int read_code(unsigned* pycbuf, struct code_obj* pobj, int cur, int size);
struct obj* init_obj(void)
{
    struct obj* pobj = malloc(sizeof *pobj);
    pobj->type = 69;
    return pobj;
}

void store_val(struct field* pycfield, int data)
{
    int length = pycfield->length;
    if (length < MAXVAL)
        pycfield->val[length++] = data;
    pycfield->length++;
}

int find_start_of_this_code(unsigned* pycbuf, int cur)
{
    int nxt;
    while(pycbuf[cur] != TYPE_CODE && cur > 0)
        cur--;
    if (cur <= 0)
	nxt = 0;
        for(;!(pycbuf[nxt] == TYPE_CODE && pycbuf[nxt+17] == TYPE_STRING);nxt++);
    	nxt = nxt+22;
	return nxt;
    return cur + 22;
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
    while(cur < end) {
        if(pycbuf[cur] > 90) 
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

int get_code(unsigned* pycbuf, struct code_obj* pobj, int cur, int size)
{
    int func_idx = 0;
    int op_arg = 0;
    int num_obj = 1;
    int len = pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24; 
    int end = cur + len;
    while (cur < end-1 && !(cur >= size)) 
    {
	if(pycbuf[cur] == LOAD_CONSTANT && pycbuf[cur+3] == MAKE_FUNCTION)
	{
	    int nxt = cur,num = num_obj;
	    while(num--) 
	    {
        	nxt++;
		for(;!(pycbuf[nxt] == TYPE_CODE && pycbuf[nxt+17] == TYPE_STRING);nxt++);
    		nxt = nxt+22;
    	    }
            struct code_obj* pnew_obj = malloc(sizeof(*pnew_obj));
            func_idx = read_code(pycbuf,pnew_obj,nxt,size); 
            objects[func_idx]->val.pobj = pnew_obj;
            objects[func_idx]->type = TYPE_CODE;
            num_obj++;
            cur += 9; 
        }
	else 
	{
            store_val(&(pobj->code),pycbuf[cur]);
            if(pycbuf[cur] > 90)  //Arguments
	    {
      		int oparg = pycbuf[cur+1] | pycbuf[cur+2] << 8; 
    		int opcode = pycbuf[cur];
    		if (opcode >= JUMP_FORWARD && opcode <= POP_JUMP_IF_TRUE)
       			oparg = compute_offset(pycbuf, cur, oparg);
    		op_arg = oparg;
                store_val(&(pobj->code),op_arg);
                cur += 3;
            } 
	    else 
	    {
                cur += 1; 
            }
        } 
    }
    store_val(&(pobj->code), pycbuf[cur]);
    return cur+1; 
}

int pos(unsigned* pycbuf, int cur)
{
    cur += 17; 
    cur += (pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24) + 5;
    int n_const = pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24;
    cur += 5;
    while(n_const--) 
    {
        if(pycbuf[cur] == 69)
            cur += 5;
        else if(pycbuf[cur] == TYPE_NONE)
            cur += 1;
        else if(pycbuf[cur] == TYPE_CODE)
            cur = pos(pycbuf, cur);
        else 
            cur++;
    }
    n_const = 0;
    while(1) 
    {
            if(pycbuf[cur] == 28)
                n_const++;
            if(n_const == 4)
                break;
            cur++; 
    }
    cur += 5;
    cur += (pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24) + 5;
    cur += (pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24) + 5;
    cur += 4;
    cur += (pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24) + 5;
    return cur;
}

int get_consts(unsigned* pycbuf, struct code_obj* pobj, int cur, int size)
{
    int no_consts = pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24; 
    int count = 0;
    cur += 5;
    while(count < no_consts) 
    {
        if(pycbuf[cur] == 69) 
	{ 
	    int l = pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24;
            store_val(&(pobj->consts),l);
            cur += 5;
        }
        else if(pycbuf[cur] == TYPE_NONE)
	{
            store_val(&(pobj->consts),0);
            cur += 1;
        }
	else if(pycbuf[cur] == TYPE_CODE) 
	{
            store_val(&(pobj->consts),0);
            cur = pos(pycbuf, cur);
        }
        count++;
    }
    return cur; 
}

int get_names(unsigned* pycbuf, struct code_obj* pobj, int cur)
{
    int n_names = pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24;
    int func_idx = 0;
    struct obj* ptrobj;
    cur += 5;
    while(n_names--) 
    {
        if (pycbuf[cur] == TYPE_INTERN) 
	{
            ptrobj = malloc(sizeof *ptrobj);
           // copy(ptrobj->name,pycbuf,cur+5,length(pycbuf,cur));
	    char *s = ptrobj->name;
	    int l = pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24;
	    while(l--)
	    {
		*s = pycbuf[cur++];
		s++;
	    }
	    *s = '\0';
            objects[obj_cnt++] = ptrobj;             
            pobj->names[pobj->name_cnt++] = ptrobj; 
            cur += (pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24) + 5;
        } 
	else if (pycbuf[cur] == TYPE_SREF) 
	{
            func_idx = pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24;
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
    int n_varnames = (pobj->varnames).length = pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24;
    cur += 5;
    while(n_varnames--) 
    {
        if (pycbuf[cur] == TYPE_INTERN) 
	{
            ptrobj = init_obj();
            //copy(ptrobj->name, pycbuf, cur+5, length(pycbuf, cur));
	    char *s = ptrobj->name;
	    int l = pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24;
	    while(l--)
	    {
		*s = pycbuf[cur++];
		s++;
	    }
	    *s = '\0';
            objects[obj_cnt++] = ptrobj;        
            cur += (pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24) + 5;
        } 
	else if (pycbuf[cur] == TYPE_SREF) 
	{
            cur += 5;
        } 
	else
            cur++;
    }
    return cur;
}

int code_name(unsigned* pycbuf, struct code_obj* pobj, int cur)
{
    int n_field = 0,ret;
    struct obj* ptrobj;
    while(1) 
    {
            if (pycbuf[cur] == 28)
                n_field++;
            if (n_field == 2)
                break;
            cur++; 
    }
    cur += 5;
    cur += (pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24) + 5;
    if (pycbuf[cur] == TYPE_INTERN) 
    {
        ptrobj = init_obj();
	int l = pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24;
 	// copying to name
        strncpy(ptrobj->name,(char *)&pycbuf[cur+5],l+1);
        objects[obj_cnt++] = ptrobj;
        pobj->name = ptrobj->name;
        return obj_cnt-1;

    } 
    else 
        ret = pycbuf[cur+1] | pycbuf[cur+2] << 8 | pycbuf[cur+3] << 16 | pycbuf[cur+4] << 24;
	return ret;
}

int read_code(unsigned* pycbuf, struct code_obj* pobj, int cur, int size)
{
    cur = get_code(pycbuf, pobj, cur, size);
    cur = get_consts(pycbuf, pobj, cur, size);
    cur = get_names(pycbuf, pobj, cur);
    cur = get_varnames(pycbuf, pobj, cur);
    code_name(pycbuf, pobj, cur);
}
