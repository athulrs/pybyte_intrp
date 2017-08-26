#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<time.h>
#define SIZE 256
int stack_size;
int *stack;
int store_array[256];
int status =0;
int condition;
int make_counter =0;
int fn_instructions[SIZE];
int call_func =0;
int function_position =0;
int push_w(int, int, int*, int*,int);
int pop_w(int, int, int*, int*);
void interpreter_loop(int*, int*,int,int);
void push_stack(int);
int pop(void);
int bsum(int, int);
int bsub(int , int);
int bmul(int , int);
int bdiv(int, int);
int bmod(int, int);
int loop_if(int, int*,int*, int);
int loop_while(int, int*, int*,int);
int run_func(int,int,int*,int*,int);
struct function
{
  int fn_body[SIZE];
  int ln_of_fn_body;
  int fn_name[SIZE];
  int ln_of_name;
  int local_constants[SIZE];
  int ln_local_constants;
  int cnt_local_variables;
  int local_vars_names[SIZE];
  int module_name[SIZE];  
}fun[SIZE];
int *const_array;
int *dup_const_array[SIZE];
int *name_array;
int instruction_array[SIZE *8];
int lenofinstr=0,k=0,l=0;
int struct_index=0;
int no_of_funs =0;
int i=0,counter=0,len_const_array = 0;
char *var_array[SIZE];
int no_fns;
char* strrev(char *);
int push(int);
void start_interpreter(int*, int*);
int htod(char*);
unsigned char *next_byte(unsigned char*,char *opcode);
void print_var_array(int);
unsigned char *create_var_array(char*, int, unsigned char*, int);
int main()
{
    FILE *ptr;
    ptr = fopen("test1.pyc","rb"); 
    size_t n;
    time_t time;
    int start=0, counter_local_const=0;
    int len,check=0,counter=0,opcode_seventyfour=0,counter_mgc=4,counter_ts=4;
    unsigned char buffer[2],hex_str[SIZE*8];
    unsigned char *hex_val = hex_str;
    char opcode[3],values[8],timestamp[8],values1[8],values2[8],values3[8],values4[8],values5[8],values6[8],magic[8];
    int i,j;
    if(ptr)
    {
     while(!feof(ptr))
	{
	fread(buffer,sizeof(char),1,ptr);
	hex_val += sprintf(hex_val,"%02X",(unsigned char)buffer[0]); //converting bin to hex 
        }
    }
    *(hex_val+1) ='\0';
    fclose(ptr);
    hex_val = hex_str;
    printf("\n");
   //magic num
   i =counter_mgc;
   while(i>0)
   {
    hex_val = next_byte(hex_val,opcode);
    if(i==counter_mgc)
	strcpy(magic,opcode);
    else
	strcat(magic,opcode);  
    i--;
   }
   //timestmp
   i= counter_ts;
    while(i>0)
    {
     hex_val = next_byte(hex_val,opcode);
     if(i==counter_mgc)
	strcpy(timestamp,opcode);
     else
	strcat(timestamp,opcode);   
     i--;
    }
   i=16;
   while(i>0)
   {
     hex_val = next_byte(hex_val,opcode);
     i--;	
   }
   hex_val = next_byte(hex_val,opcode);
   hex_val = next_byte(hex_val,opcode);
   if(strcmp(opcode,"73")==0 && start==0)
   {
      start =1;
      for(i=4;i>0;i--)
      {
	hex_val = next_byte(hex_val,opcode);
	if(i==4)
	   strcpy(values,strrev(opcode));
	else
	   strcat(values,strrev(opcode));
      }
   }    
   lenofinstr=htod(strrev(values));
   i=0;
   j=0;
   while(*hex_val)
   {
      hex_val = next_byte(hex_val,opcode); 
      if(i<lenofinstr)
      {
	instruction_array[i]=htod(opcode);
	if(strcmp(opcode,"84")==0) 
	   no_fns++;
	no_of_funs = no_fns;
      }
      else if(strcmp(opcode,"73")==0 && no_fns >0)
      { 
	int function_body=0,local_vars=0, module_name=0,fn_name=0,counter =0,lenofargmnts=0;
        for(j=4;j>0;j--)
        {
    	  hex_val = next_byte(hex_val, opcode);
   	   if(j==4)
  	     strcpy(values2,strrev(opcode));
  	   else
       	     strcat(values2,strrev(opcode));
        }
        int lenofarg =htod(strrev(values2));			      
        fun[struct_index].ln_of_fn_body =lenofarg; 
        counter = lenofarg;
        while(counter > 0){
        hex_val = next_byte(hex_val, opcode);
        fun[struct_index].fn_body[k] =htod(opcode); 
        k++;
        counter--;
      }
      hex_val = next_byte(hex_val,opcode);
      while(strcmp(opcode,"74") != 0)
	{ 
	      if(strcmp(opcode,"69") == 0)
	       {    
	       	 for(l=4;l>0;l--)
		 {	 
		  hex_val = next_byte(hex_val, opcode);
	          if(l==4)
                    strcpy(values6,strrev(opcode));
	          else
	            strcat(values6,strrev(opcode));
	         }
	        fun[struct_index].local_constants[counter_local_const] = htod(strrev(values6));
	        counter_local_const++;
	       }
	    fun[struct_index].ln_local_constants = counter_local_const;
	    hex_val = next_byte(hex_val,opcode);
	}
     int total_no_of_local_vars =0;
     while(strcmp(opcode,"74") ==0)
     {  
	 for(l=4;l>0;l--)
	 {
	     hex_val = next_byte(hex_val, opcode);
	     if(l==4)
          	   strcpy(values3,strrev(opcode));
	     else
	         strcat(values3,strrev(opcode));
	 }
	 lenofargmnts = htod(strrev(values3));
	 l=0;
	 while(lenofargmnts >0)
	 {
	      hex_val = next_byte(hex_val,opcode);
              fun[struct_index].local_vars_names[l] = htod(opcode); 
	      l++;
	      lenofargmnts--;
         }
	 total_no_of_local_vars++;
	 hex_val = next_byte(hex_val,opcode);
     }
	fun[struct_index].cnt_local_variables = total_no_of_local_vars;
        hex_val = next_byte(hex_val, opcode);
	while(strcmp(opcode,"73") !=0) 
        {
	  hex_val = next_byte(hex_val, opcode);
	}
	l=0;
	for(l=4;l>0;l--)
	{
	   hex_val = next_byte(hex_val, opcode);
	   if(l==4)
		strcpy(values4,strrev(opcode));
	   else
	        strcat(values4,strrev(opcode));
	}
	lenofargmnts =0;
	lenofargmnts = htod(strrev(values4));
        l=0;
        while(lenofargmnts >0)
	{	
	 hex_val = next_byte(hex_val,opcode);
	 fun[struct_index].module_name[l] = htod(opcode); 
	 l++;
	 lenofargmnts--;
        }
      hex_val = next_byte(hex_val,opcode);
      while(strcmp(opcode,"74") !=0)
      {
	hex_val = next_byte(hex_val,opcode);
      }
      l=0;
      for(l=4;l>0;l--)
	{
	 hex_val = next_byte(hex_val, opcode);
	 if(l==4)
              strcpy(values5,strrev(opcode));
	 else
	      strcat(values5,strrev(opcode));
        }
      lenofargmnts =0;
      lenofargmnts = htod(strrev(values5));
      fun[struct_index].ln_of_name = lenofargmnts; 
      l=0;
      while(lenofargmnts >0)
      {
	hex_val = next_byte(hex_val,opcode);
	fun[struct_index].fn_name[l]; 
	l++;
	lenofargmnts--;
      }
      no_fns--;
      struct_index++;
      }
      else if(strcmp(opcode,"69")==0)
      {
	 for(j=4;j>0;j--)
	 {
          hex_val = next_byte(hex_val, opcode);
	  if(j==4)
            strcpy(values1,strrev(opcode));
	  else
	    strcat(values1,strrev(opcode));
         }
	len_const_array = push(htod(strrev(values1)));
       }
      i++;
      }
   start_interpreter(instruction_array, const_array);
}

void start_interpreter(int *instruction_array, int *const_array)
{
  int fn_status =0;
  interpreter_loop(instruction_array, const_array, lenofinstr, fn_status);
}

void interpreter_loop(int *instructions, int *constants, int length, int fn_status)
{
  int opc;
  for(int i=0;i<length;i++)
  {
    opc=instructions[i];
    switch(opc)
    {
    	case 100: //load cnstnt
       		i= push_w(opc,i,instructions,constants,fn_status);
      		break;
    	case 90: //str name
    	  	i=pop_w(opc,i,instructions,constants);
   	   	break;
    	case 101: //load name
      		i= pop_w(opc,i,instructions,store_array);
      		break;
	case 20: //mul
	        pop_w(opc,i,instructions,store_array);
      		break;
    	case 21: //div
     		pop_w(opc,i,instructions,store_array);
      		break;
    	case 22: //mod
     		pop_w(opc,i,instructions,store_array);
      		break;
    	case 23: //sum
      		pop_w(opc,i,instructions,store_array);
      		break;
    	case 24: //sub
      		pop_w(opc,i,instructions,store_array);
      		break;
    	case 107: //if
      		i = loop_if(i,instructions,store_array,status);
      		break;
    	case 113: //while loop
      		i=loop_while(i,instructions,store_array,fn_status);    
      		break;
    	case 71: //print fnc
      		printf("%d\n",pop());
      		break;
    	case 131: // fnc call
     	 	i=push_w(opc,i,instructions,store_array,fn_status);
      		fn_status =0;
      		break;
    	case 124: // load fst 
      		i = push_w(opc,i,instructions,store_array,fn_status);
      		break;
    	case 125: //str fst
      		i = push_w(opc, i,instructions,store_array,fn_status);
     		break;
    }
  }
}

int  push_w(int ins,int pos,int* instructions,int* constants,int fn_status)
{
  if(ins == 100)
  { 
	if(instructions[pos+3] == 132)
	{ 
		push_stack(instructions[pos+1]);
	}
	else
	{
	  if(fn_status ==0)
  	  {  
		push_stack(constants[instructions[pos+1] - no_of_funs]);	  
           	pos =pos+2;
 	  }
	  else if(fn_status ==1)
	  {
	  	push_stack(fun[function_position].local_constants[instructions[pos+1] -len_const_array]);
       	        pos = pos+2;
	  }
        }
  }
  else if(ins == 131)
  {
	int l;   
        int  temp =pos,args_length=0,hop=0,which_function;
        call_func =pos;
        pos = pos+1;
        args_length = instructions[pos];
        hop = (args_length+1); 
        while(hop>0)
	{
         pos = pos-3;
         push_stack(store_array[instructions[pos]]);
         hop--;
        }
        which_function = pop();
        function_position = which_function;
        pos = run_func(which_function,call_func,instructions,store_array,args_length);
  }
  else if(ins == 124)
  { 
	push_stack(const_array[instructions[pos+1]]);
    	pos = pos+2;
  }
  else if(ins == 125)
  {
    	int next_pos = pos+1;
    	int val= instructions[next_pos];
    	int top = pop();
    	const_array[val] = top;
    	pos = pos+2;
  }
  return pos;
}

int pop_w(int ins, int pos, int* instructions, int *constants)
{
  if(ins == 90)
  { 
    int val=0,next;
    val=pop();
    next=instructions[pos+1];
    store_array[next]=val;
    pos=pos+2;  
  }
  else if(ins == 101)
  {  
    int val =0,next;
    next=instructions[pos+1];
    push_stack(store_array[next]);
    pos=pos+2;
  }
  else if (ins == 22)
  { 
    int bottom =0, top=0;
    top = pop();
    bottom = pop();
    push_stack(bmod(bottom,top));
  }
  else if(ins == 23)
  { 
    int bottom =0, top=0;
    top =pop();
    bottom =pop();
    push_stack(bsum(top,bottom)); 
  }
  else if(ins == 24)
  { 
    int bottom=0,top=0;
    top=pop();
    bottom=pop();
    push_stack(bsub(bottom,top));
  } 
  else if(ins == 20)
  {  
    int bottom =0,top=0;
    top=pop();
    bottom = pop();
    push_stack(bmul(top,bottom));
  }
  else if(ins == 21)
  { 
    int top=0,bottom =0;
    top=pop();
    bottom =pop();
    push_stack(bdiv(bottom,top));
  }
  return pos;
}

int loop_if(int pos,int *instructions,int *constants,int fn_status)
{
  int t=0,b=0,pos1;
  int oper;
  oper=instructions[pos+1];
  t=pop();
  b=pop(); 
  switch(oper)
  {
   // <
   case 0: 
	if(b<t)
	{     
	  pos=pos+2;
    	}
	else
	{
     	  if(instructions[pos-9] == 120)
	  { 
            int hop=instructions[pos-8];
            return hop+pos;
      	  }
	  else
	  {
	    pos=instructions[pos+4]-1;
          }
        }
    	break;
  // <=
  case 1: 
   	 if(b<=t)
	 {
           pos =pos+2;
         }
 	 else
	 {
      	  if(instructions[pos-9] == 120)
	  { 
            int hop = instructions[pos-8];
            return hop+pos;
     	  }
	  else
	  {
            pos =  instructions[pos+4]-1;
          }
  	}
    	break;
  // ==
  case 2: 
    	if(b==t)
	{
          pos =pos+2;
    	}
	else
	{
          if(instructions[pos-9] == 120)
	  { 
            int hop = instructions[pos-8];
            return hop+pos;
      	  }
	  else
	  {
	    pos =  instructions[pos+4]-1;
          }
     	}
    	break;
  // !=
  case 3: 
       	if(b!=t)
	{
      	  pos = pos+2;
    	}
	else
	{
          if(instructions[pos-9] == 120)
	  { 
            int hop = instructions[pos-8];
            return hop+pos;
      	  }
	  else
	  {
  	    pos =  instructions[pos+4]-1;
          }
     	}
   	break;
  // > 
  case 4: 
	if(b>t)
	{
          pos = pos+2; 
     	}
	else
	{
      	  if(instructions[pos-9] == 120)
	  { 
           int hop = instructions[pos-8];
	   if(fn_status ==1)
           	return hop+(pos-9);
	   else
	  	return hop+(pos-9);
          }
	  else
	  {
	   pos =  instructions[pos+4]-1;	
       	  }
    	}
  	break;
  // >=
  case 5: 
    	if(b>=t)
	{
      	  pos =pos+2;
    	}
	else
	{
       	  if(instructions[pos-9] == 120)
	  { 
            int hop = instructions[pos-8];
            return hop+pos;
          }
	  else
	  {
	    pos =  instructions[pos+4]-1;
          }
        }
    	break;
  } 
  return pos;
}

int loop_while(int pos, int* instructions, int *constants,int status)
{
  int next;
  next = instructions[pos+1];
  pos = next;
  return pos-1;
}

int run_func(int fun_pos, int old_pos, int* instructions, int* store_array, int no_of_args)
{
    int len_of_fn = fun[fun_pos].ln_of_fn_body, in_function;
    int l=0;
    for(l=0;l<len_of_fn-1;l++)
    {
      fn_instructions[l] = fun[fun_pos].fn_body[l];
    }
    in_function =1;
    interpreter_loop(fn_instructions,const_array,len_of_fn, in_function); 
    return call_func+3; 
}

//convertion 16 to 10
int htod(char *hex)
{
  int len,decimal=0,val,i=0;
  len = strlen(hex);
  len--;
  for(i=0;hex[i]!='\0';i++)
  {
   if(hex[i]>='0' && hex[i]<='9')
    {
     val=hex[i]-48;
    }
    else if(hex[i]>='a' && hex[i]<='f')
    {
     val=hex[i]-97+10;
    }
    else if(hex[i]>='A' && hex[i]<='F')
    {
     val=hex[i]-65+10;
    }
    decimal+=val*pow(16,len);
    len--;
  }
  return decimal;
}
 
char* strrev(char *str)
{
    int start,end,len;
    char temp,*ptr0=NULL;
    len=strlen(str);  
    ptr0=malloc(sizeof(char)*(len+1));
    if(ptr0 == 0)
    {
      exit(1);
    }
    ptr0 = strcpy(ptr0,str);           
    for(start=0,end=len-1;start<=end;start++,end--)
    {
        temp = ptr0[start];
        ptr0[start] = ptr0[end];       
        ptr0[end] = temp;
    }
    return ptr0; 
}

unsigned char *next_byte(unsigned char *hex_val,char *opcode)
{
     opcode[0] = *hex_val;
     opcode[1] = *(hex_val +1);
     opcode[2] = '\0';
     hex_val++;
     hex_val++;
     return hex_val;  
}

int bsum(int a,int b)
{
  return a+b;  
}

int bsub(int a,int b)
{
  return a-b;
}

int bmul(int a,int b)
{
  return a*b;
}

int bdiv(int a,int b)
{
  return a/b;
}

int bmod(int a,int b)
{
  return a%b;
}

int push(int a)
{
  int  *ptr2;
  static int i=0;
  if(i==0)
  {
    const_array =(int*)malloc(sizeof(int) * 1);
    if(const_array == 0)
    {
      exit(1);
    }
    *const_array = a;
  }
  else
  {
    ptr2 = (int*)realloc(const_array,sizeof(int) *i+1);
    if(ptr2==0)
    {
      free(const_array);
      exit(1);
    }
    *(ptr2 + i) =a;
  }
  i= i+1;
  return i;
}

int pop()
{
  int top;
  int index = stack_size -1;
  top = stack[index];
  stack_size = stack_size -1;
  return top;
}

void push_stack(int a)
{
  static int l=0;
  int *ptr3;
  if(l==0)
  {
    stack=(int*)malloc(sizeof(int)*1);
    if(stack == 0)
    {
      exit(1);
    }
    *stack = a;
  }
  else
  {
    ptr3=(int*)realloc(stack,sizeof(int)*l+1);
    if(ptr3==0)
    {
      free(stack);
      exit(1);
    }
    *(ptr3 + l) =a;
  }
  l= l+1;
  stack_size = l;   
}
