#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static int calc(char method, va_list ap, int *res)
{
	int op1, op2;

	op1 = va_arg(ap, int);
	op2 = va_arg(ap, int);

	switch (method) {
	case '+':
		*res = op1 + op2;
		break;
	case '-':
		*res = op1 - op2;
		break;
	case '*':
		*res = op1 * op2;
		break;
	case '/':
		*res = op1 / op2;
		break;
	default:
		return -1;
	}

	return 0;
}

static void get(char method, ...)
{
	int ret, res;
	va_list ap;

	va_start(ap, method);
	ret = calc(method, ap, &res);
	va_end(ap);

	if (ret < 0) {
		fprintf(stderr, "Invalid operator\n");
		return;
	}

	printf("%d\n", res);
}

int main(int argc, char **argv)
{
	if (argc != 4)
		return 1;

	char method;
	int op1, op2;

	method = argv[1][0];
	op1 = atoi(argv[2]);
	op2 = atoi(argv[3]);

	printf("%d %c %d = ", op1, method, op2);
	fflush(stdout);
	get(method, op1, op2);

	return 0;
}
