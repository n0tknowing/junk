/* WIP: my simple implementation of stack-based virtual machine */

#include <stdint.h>
#include <stdio.h>

/* instruction pointer (or pc) */
static uint8_t ip = 0;
static uint8_t running = 1;
static uint8_t sp = -1;
static uint8_t stack[UINT8_MAX];

/* opcodes */
enum {
	PSH,  /* push */
	POP,  /* pop */
	ADD,  /* addition */
	MUL,  /* multiply */
	HLT  /* halt */
};

static const uint8_t program[] = {
	PSH, 5,
	PSH, 11,
	MUL,
	PSH, 4,
	MUL,
	POP,
	HLT
};

static void eval(int instr)
{
	uint8_t a, b, res, val;

	switch (instr) {
	case ADD:
		/* first we pop the stack and stores it as 'a' */
		a = stack[sp--];
		/* then pop the top of stack and stores it as 'b' */
		b = stack[sp--];
		res = a + b;
		sp++;
		stack[sp] = res;
		break;
	case MUL:
		a = stack[sp--];
		b = stack[sp--];
		res = a * b;
		sp++;
		stack[sp] = res;
		break;
	case PSH:
		sp++;
		stack[sp] = program[++ip];
		break;
	case POP:
		val = stack[sp--];
		printf("popped %d\n", val);
		break;
	case HLT:
		running = 0;
		break;
	default:
		fprintf(stderr, "invalid instruction: %d\n", instr);
		running = 0;
		break;
	}
}

int main(void)
{
	while (running) {
		eval(program[ip]);
		ip++;
	}
}
