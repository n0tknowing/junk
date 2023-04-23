#include <stdio.h>

static int is_balanced(const char *str)
{
	int parens = 0, invalid = 0;

	for (int i = 0; str[i]; i++) {
		switch (str[i]) {
		case '(':
			parens++;
			break;
		case ')':
			if (parens == 0)
				return 0;
			parens--;
			break;
		default:
			invalid++;
			break;
		}
	}

	return invalid ? 0 : parens == 0;
}

int main(int argc, char **argv)
{
	if (argc < 2)
		return 1;

	for (int i = 1; i < argc; i++) {
		const char *str = argv[i];
		int res = is_balanced(str);
		printf("%s is%s balanced\n", str, res ? "" : " not");
	}

	return 0;
}
