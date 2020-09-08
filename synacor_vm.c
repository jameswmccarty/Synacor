#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#define BUFFMAX 30

/* Stack Prototypes */
typedef struct _element {
	struct _element *prev;
	unsigned short int item; /* Integer stack */
} element;

typedef struct _stack {
	element *top;
} stack;

int push(stack* stk, unsigned short int a);
int pop(stack* stk);

/***********************/
/* Global VM Variables */
/***********************/
int ip;
/* eight registers */
/* numbers 32768..32775 instead mean registers 0..7 */
unsigned short int mem[32776] = {0};
/* an unbounded stack which holds individual 16-bit values */
/* empty stack = error (-1) */
stack vm_stack;
char stdinbuff[BUFFMAX] = {0};

/* Adds an item to the top of a stack */
int push(stack* stk, unsigned short int a) {
	element *new; /* new element to push to stack */	
	if(stk == NULL) /* can't do anything */
		return -1;
	new = (element *) malloc(sizeof(struct _element));
	if(new == NULL) /* malloc fails */
		return -1;
	new->item = a;
	/* set items prev pointer to top of current stack */
	new->prev = stk->top;
	/* put this item on top of the stack */
	stk->top = new;
	return 0;
}

/* pop an item off the stack */
int pop(stack* stk) {
	/* pointer to current top of stack */
	element *current;
	int temp; /* data inside of this element */
	if(stk->top == NULL)
		return -1;
	/* get current top of stack */
	current = stk->top;
	/* set new top of stack to item below current */
	stk->top=current->prev;
	/* get the data from the current element */
	temp = current->item;
	/* free the old element */
	free(current);
	return temp;
}

int is_reg(short unsigned int x) {
	if(32768 <= x && x <= 32775)
		return 1;
	return 0;
}

int main(int argc, char **argv) {

	FILE *infile;
	short int b, c; /* temp vars */
	int in_idx = 0; /* index input buffer */
	
	/* read input file into the mem buffer */
	
	infile = fopen("challenge.bin","rb");
	if(!infile) {
		printf("Unable to open file!\n");
		return 1;
	}
	fseek(infile, 0L, SEEK_END);
	ip = (int) ftell(infile);
	fseek(infile, 0L, SEEK_SET);
	if(fread(&mem, sizeof(unsigned short int), ip/2, infile) != ip/2)
		return -1;
	for(ip = 32768; ip <= 32775; ip++)
		mem[ip] = 0;
	ip = 0;

	while(1) {
		 /* printf("%u %u\n", ip, mem[ip]); */
		/*printf("%d, %d, %d, %d, %d, %d, %d, %d\n",
			mem[32768],mem[32769],mem[32770],mem[32771],mem[32772],mem[32773],mem[32774],mem[32775]); */
		switch(mem[ip]) { /* read OPCODE */
			case 0: /* halt */
				return 0;
			case 1: /* set */
				mem[mem[ip+1]] = is_reg(mem[ip+2]) ? mem[mem[ip+2]] : mem[ip+2];
				ip += 3;
				break;
			case 2: /* push */
				if(is_reg(mem[ip+1]))
					push(&vm_stack, mem[mem[ip+1]]);
				else
					push(&vm_stack, mem[ip+1]);
				ip += 2;
				break;
			case 3: /* pop */
				b = pop(&vm_stack);
				if(b == -1)
					return -1;
				mem[mem[ip+1]] = (unsigned short int) b;
				ip += 2;
				break;
			case 4: /* eq */
				b = is_reg(mem[ip+2]) ? mem[mem[ip+2]] : mem[ip+2];
				c = is_reg(mem[ip+3]) ? mem[mem[ip+3]] : mem[ip+3];
				mem[mem[ip+1]] = (c == b) ? 1 : 0;
				ip += 4;
				break;
			case 5: /* gt */
				b = is_reg(mem[ip+2]) ? mem[mem[ip+2]] : mem[ip+2];
				c = is_reg(mem[ip+3]) ? mem[mem[ip+3]] : mem[ip+3];
				mem[mem[ip+1]] = (c < b) ? 1 : 0;
				ip += 4;
				break;
			case 6: /* jmp */
				if(is_reg(mem[ip+1]))
					ip = mem[mem[ip+1]];
				else
					ip = mem[ip+1];
				break;
			case 7: /* jt */
				if((is_reg(mem[ip+1]) && mem[mem[ip+1]] != 0) ||
					(!is_reg(mem[ip+1]) && mem[ip+1] != 0))
						ip = is_reg(mem[ip+2]) ? mem[mem[ip+2]] : mem[ip+2];
				else
					ip += 3;
				break;
			case 8: /* jf */
				if((is_reg(mem[ip+1]) && mem[mem[ip+1]] == 0) ||
					(!is_reg(mem[ip+1]) && mem[ip+1] == 0))
						ip = is_reg(mem[ip+2]) ? mem[mem[ip+2]] : mem[ip+2];
				else
					ip += 3;
				break;
			case 9: /* add */
				b = is_reg(mem[ip+2]) ? mem[mem[ip+2]] : mem[ip+2];
				c = is_reg(mem[ip+3]) ? mem[mem[ip+3]] : mem[ip+3];
				mem[mem[ip+1]] = (unsigned short int) ((b + c) % 32768);
				ip += 4;
				break;
			case 10: /* mult */
				b = is_reg(mem[ip+2]) ? mem[mem[ip+2]] : mem[ip+2];
				c = is_reg(mem[ip+3]) ? mem[mem[ip+3]] : mem[ip+3];
				mem[mem[ip+1]] = (unsigned short int)  ((b * c) % 32768);
				ip += 4;
				break;
			case 11: /* mod */
				b = is_reg(mem[ip+2]) ? mem[mem[ip+2]] : mem[ip+2];
				c = is_reg(mem[ip+3]) ? mem[mem[ip+3]] : mem[ip+3];
				mem[mem[ip+1]] = (unsigned short int) ((b % c) % 32768);
				ip += 4;
				break;
			case 12: /* and */
				b = is_reg(mem[ip+2]) ? mem[mem[ip+2]] : mem[ip+2];
				c = is_reg(mem[ip+3]) ? mem[mem[ip+3]] : mem[ip+3];
				mem[mem[ip+1]] = (unsigned short int) (b & c & 0xffff);
				ip += 4;
				break;
			case 13: /* or */
				b = is_reg(mem[ip+2]) ? mem[mem[ip+2]] : mem[ip+2];
				c = is_reg(mem[ip+3]) ? mem[mem[ip+3]] : mem[ip+3];
				mem[mem[ip+1]] = (unsigned short int) (b | c);
				ip += 4;
				break;
			case 14: /* not */
				b = is_reg(mem[ip+2]) ? mem[mem[ip+2]] : mem[ip+2];
				mem[mem[ip+1]] = (unsigned short int) (~b & 0x7fff);
				ip += 3;
				break;
			case 15: /* read mem */
				b = is_reg(mem[ip+2]) ? mem[mem[mem[ip+2]]] : mem[mem[ip+2]];
				if(is_reg(mem[ip+1]))
					mem[mem[ip+1]] = b;
				ip += 3;
				break;
			case 16: /* write mem */
				b = is_reg(mem[ip+2]) ? mem[mem[ip+2]] : mem[ip+2];
				if(is_reg(mem[ip+1]))
					mem[mem[mem[ip+1]]] = b;
				else
					mem[mem[ip+1]] = b;
				ip += 3;
				break;
			case 17: /* call */
				push(&vm_stack, (short unsigned int) ip+2);
				ip = is_reg(mem[ip+1]) ? mem[mem[ip+1]] : mem[ip+1];
				break;
			case 18: /* return */
				b = pop(&vm_stack);
				if(b == -1)
					return 0;
				ip = is_reg(b) ? mem[b] : b;
				break;
			case 19: /* out */
				b = is_reg(mem[ip+1]) ? mem[mem[ip+1]] : mem[ip+1];
				printf("%c", (char)b);
				ip += 2;
				break;
			case 20: /* in */
				if(in_idx == 0 || stdinbuff[in_idx] == '\0') {
					for(b=0; b<BUFFMAX; b++) {
						stdinbuff[b] = '\0';
					}
					putc('>', stdout);
					if(fgets(stdinbuff, BUFFMAX-1, stdin)==NULL)
						return -1;
				}
				mem[mem[ip+1]] = (unsigned short int) stdinbuff[in_idx];
				in_idx++;
				if(in_idx == BUFFMAX-1 || stdinbuff[in_idx] == '\0' || (in_idx > 0 && stdinbuff[in_idx-1] == '\n'))
					in_idx = 0;
				ip += 2;
				break;
			case 21: /* noop */
				ip += 1;
				break;
			default:
				return 1;
		}
		ip %= 32768;
	}
	return 0;
}





