#define LOAD_CONSTANT 0x64
#define LOAD_NAME 0x65
#define STORE_NAME 0x5a
#define PRINT_ITEM 0x47
#define PRINT_NEWLINE 0x48
#define COMPARE_OP 0x6b
#define BINARY_ADD 0x17
#define BINARY_MULTIPLY 0x14
#define BINARY_DIVIDE 0x15
#define BINARY_SUBTRACT 0x18
#define BINARY_MODULO 0x16
#define POP_JUMP_IF_FALSE 0x72
#define POP_JUMP_IF_TRUE 0x73
#define JUMP_FORWARD 0x6e
#define JUMP_ABSOLUTE 0x71
#define SETUP_LOOP 0x78
#define MAKE_FUNCTION 0x84
#define RETURN_VALUE 0x53
#define CALL_FUNCTION 0x83
#define LOAD_FAST 0x7c
#define STORE_FAST 0x7d
#define LOAD_GLOBAL 0x74
#define POP_TOP 0x1

#define TYPE_STRING 0x73
#define TYPE_CODE 0x63
#define TYPE_NONE 0x4e
#define TYPE_INTERN 0x74
#define TYPE_SREF 0x52
#define FUNCTION_START 0x43

#define TRUE 1
#define FALSE 0
#define MAXVAL 500
#define SIZE 1
#define NUM 4
#define MAXPYC 1000

struct field {
    int val[MAXVAL];
    int length;    
};

struct code_obj 
{
    char* name;
    struct field code;
    struct field consts;
    struct field varnames;
    struct obj* names[10];
    int name_cnt;
};

union value {
    int intval; //store num
    struct code_obj* pobj; //store str
};

struct obj {
    union value val;
    char name[20];
    int type;
};

void push(int data);
int pop(void);
int get_top_n(int n);
struct obj* objects[15];
void execute(struct code_obj* pobj);
