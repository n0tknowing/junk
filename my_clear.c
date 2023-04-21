#include <stdio.h>

int main(void)
{
	return printf("\033[H\033[J") != 6;
}
