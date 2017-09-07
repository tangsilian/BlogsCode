#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
int token; // current token
char *src, *old_src; // pointer to source code string;
int poolsize; // default size of text/data/stack
int line; // line number
/**
step1：
add some for vritual compiler and mest to build heap
**/
int *text,//text segment
*old_text,//
*stack;//stack segment

char *data;//because data just use string 

// instructions  指令集合
enum { LEA ,IMM ,JMP ,CALL,JZ  ,JNZ ,ENT ,ADJ ,LEV ,LI  ,LC  ,SI  ,SC  ,PUSH,
       OR  ,XOR ,AND ,EQ  ,NE  ,LT  ,GT  ,LE  ,GE  ,SHL ,SHR ,ADD ,SUB ,MUL ,DIV ,MOD ,
       OPEN,READ,CLOS,PRTF,MALC,MSET,MCMP,EXIT };

	   
//词法分析
void next() {
token = *src++;
return;
}

//解析表达式
void expression(int level) {
// do nothing
}

//语法分析入口
void program() {
next(); // get next token
while (token > 0) {
printf("token is: %c\n", token);
next();
}
}


//虚拟机入口
int eval() { 
// opreation
int op,*tmp;
while(1){
	if(op==IMM) {ax=*pc++;}//将<num>放入ax
	else if(op==LC) {ax=*(char *)ax;}//将字符串存入ax
	else if(op==LI) {ax=*(int *)ax;};//load the addrint
	else if(op==SC) {*(char *)*sp++ = ax;}//
	else if(op==SI) {*(int *)*sp++ = ax;}//       
	//*sp++ ==pop
	
	else if (op == PUSH) {*--sp = ax;}                                     // push the value of ax onto the stack
	else if (op == JMP) {PC = (int*)*PC;}                                     // Jump to th address
	
	//about the JZ/JNZ
	else if (op == JZ)   {pc = ax ? pc + 1 : (int *)*pc;}                   // jump if ax is zero
	else if (op == JNZ)  {pc = ax ? (int *)*pc : pc + 1;}                   // jump if ax is zero
	
	//about the call and ret
	else if (op == CALL) {*--sp = (int)(pc+1); pc = (int *)*pc;}           // call subroutine
	//else if (op == RET)  {pc = (int *)*sp++;}                              // return from subroutine;
    
	//ent use to build a stack frame
	else if (op == ENT)  {*--sp = (int)bp; bp = sp; sp = sp - *pc++;}      // make new stack frame

	else if (op == ADJ)  {sp = sp + *pc++;}                                // add esp, <size>

	//LEV
	else if (op == LEV)  {sp = bp; bp = (int *)*sp++; pc = (int *)*sp++;}  // restore call frame and PC

	else if (op == LEA)  {ax = (int)(bp + *pc++);}                         // load address for arguments.

	//第一个参数在栈顶 另一个参数在ax
	else if (op == OR)  ax = *sp++ | ax;
	else if (op == XOR) ax = *sp++ ^ ax;
	else if (op == AND) ax = *sp++ & ax;
	else if (op == EQ)  ax = *sp++ == ax;
	else if (op == NE)  ax = *sp++ != ax;
	else if (op == LT)  ax = *sp++ < ax;
	else if (op == LE)  ax = *sp++ <= ax;
	else if (op == GT)  ax = *sp++ >  ax;
	else if (op == GE)  ax = *sp++ >= ax;
	else if (op == SHL) ax = *sp++ << ax;
	else if (op == SHR) ax = *sp++ >> ax;
	else if (op == ADD) ax = *sp++ + ax;
	else if (op == SUB) ax = *sp++ - ax;
	else if (op == MUL) ax = *sp++ * ax;
	else if (op == DIV) ax = *sp++ / ax;
	else if (op == MOD) ax = *sp++ % ax;

	else if (op == EXIT) { printf("exit(%d)", *sp); return *sp;}
	else if (op == OPEN) { ax = open((char *)sp[1], sp[0]); }
	else if (op == CLOS) { ax = close(*sp);}
	else if (op == READ) { ax = read(sp[2], (char *)sp[1], *sp); }
	else if (op == PRTF) { tmp = sp + pc[1]; ax = printf((char *)tmp[-1], tmp[-2], tmp[-3], tmp[-4], tmp[-5], tmp[-6]); }
	else if (op == MALC) { ax = (int)malloc(*sp);}
	else if (op == MSET) { ax = (int)memset((char *)sp[2], sp[1], *sp);}
	else if (op == MCMP) { ax = memcmp((char *)sp[2], (char *)sp[1], *sp);}
	else {
    printf("unknown instruction:%d\n", op);
    return -1;
	}
	
	
}
return 0;
}



//mian method
int main(int argc, char **argv)
{
int *pc, *bp, *sp, ax, cycle; // virtual machine registers	
int i, fd;
argc--;
argv++;
poolsize = 256 * 1024; // arbitrary size
line = 1;

//打开参数地址
if ((fd = open(*argv, 0)) < 0) {
printf("could not open(%s)\n", *argv);
return -1;
}
//分配空间
if (!(src = old_src = malloc(poolsize))) {
printf("could not malloc(%d) for source area\n", poolsize);
return -1;
}
// read the source file
if ((i = read(fd, src, poolsize-1)) <= 0) {
printf("read() returned %d\n", i);
return -1;
}
src[i] = 0; // add EOF character
//关闭流
close(fd);

//allcate memory for virtual machine
if(!text=old_text=malloc(poolsize)){
	printf("could not malloc(%d) for text area\n", poolsize);
	return -1;
if(!data=malloc(poolsize)){
	printf("could not malloc(%d) for data area\n", poolsize);
	return -1;
}
if(!stack=malloc(poolsize)){
	printf("could not malloc(%d) for stack area\n", poolsize);
	return -1;
}
//地址清零
memset(text,0,poolsize);
memset(data,0,poolsize);
memset(stack,0,poolsize);

//虚拟空间初始化代码    pc-》应该指向目标代码的main函数
	bp = sp = (int *)((int)stack + poolsize);
	ax = 0;
	i = 0;
    text[i++] = IMM;
    text[i++] = 10;
    text[i++] = PUSH;
    text[i++] = IMM;
    text[i++] = 20;
    text[i++] = ADD;
    text[i++] = PUSH;
    text[i++] = EXIT;
    pc = text;

program();
return eval();
}
