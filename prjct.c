#include<stdio.h>
#include<stdlib.h>
#define MAX 100
int sptr = 0; //stack ptr
int stack[MAX];
void push(int val);
int pop(void);
void add();
void sub();
void mul();
void div();
void func(FILE *file);
int main()
{ 
	int n;
 	FILE *p;
	unsigned char magic[4], moddate[4];
 	p = fopen("prgm.pyc","rb");

	fread(magic,1,4,p); //printf("\nmno. = %c",magic);
	fread(moddate,1,4,p);
	
	fseek(p,1,SEEK_CUR);
	while(p!=eof())
		func(p);
/*	if(p)
	{
	 while(!feof(p))
       	 {
	  int n = fgetc(p);
	  printf("%02x ",n);
	 }
 	} 
*/
 return 0;
}

void func(FILE *file)
{
	int n = fgetc(file); 	//printf("value %d\n",n);
	switch(n) 
	{
		case 20:
			mul();
			break;
		case 21:
			div();
			break;
		case 23:
			add();
			break;
		case 24:
			sub();
			break;
		case 71:
			printf("%d", pop());
			break;
	}
}


void push(int val)
{
	if (sptr < MAX) 
	{
	 stack[sptr++] = f;
//	 printf("Push value %d\n", f);
	}
	else 
	 printf("stack full... !!!!\n");
}

int pop(void)
{
	if (sptr > 0) 
	{
	 return stack[--sptr];
	}
	else
	{
         printf("stack empty... !!!\n");
 	 return 0;
	}
}


void add()
{
	int num1,num2;
	num1 = pop();
	num2 = pop();
	push(num1 + num2);
}

void mul()
{
	int num1,num2;
	num1 = pop();
	num2 = pop();
	push(num2 * num1);
}
void sub()
{
	int num1,num2;
	num1 = pop();
	num2 = pop();
	push(num2 - num1);
}

void div()
{
	int num1,num2;
	num1 = pop();
	num2 = pop();
	push(num2 / num1);
}



/*
int buffer[10];
FILE *ptr;
ptr = fopen("prgm.pyc","rb");  
fread(buffer,sizeof(buffer),1,ptr);
for(int i = 0; i<10; i++)
    printf("%02x", buffer[i]);
*/

/*else 
{
fseek(p,0,SEEK_END);
size = ftell(p);
rewind(p);
b = (char*)malloc(size);
items_read = fread(b,1,size,p);
fclose(p);
  for(i=0;i<size;i++){
if(b[i] == '\n')
{
linecount++;
}
}
}
printf("\nNo. of lines %d with %d chars (0 read ).\n",linecount,i);
free(b);
 for(n=1;n<5;n++)
{
 fread(&num,sizeof(struct three),1,p);
 printf("\nn1:%d \tn2:%d \tn3:%d",num.n1,num.n2,num.n3);
 }

fclose(p);
*/ 
