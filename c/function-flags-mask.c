/* like the second argument of open(2) */
#include <stdio.h>

enum {
	FLAGS_A  = 1 << 0,
	FLAGS_B  = 1 << 1,
	FLAGS_C  = 1 << 2,
	FLAGS_D  = 1 << 3
};

static void check_flags(int flags)
{
	if ((flags & FLAGS_A) == FLAGS_A)
		puts("FLAGS_A is set");
	if ((flags & FLAGS_B) == FLAGS_B)
		puts("FLAGS_B is set");
	if ((flags & FLAGS_C) == FLAGS_C)
		puts("FLAGS_C is set");
	if ((flags & FLAGS_D) == FLAGS_D)
		puts("FLAGS_D is set");
}

int main(void)
{
	int flags = FLAGS_A | FLAGS_B;
	check_flags(flags);
	return 0;
}
