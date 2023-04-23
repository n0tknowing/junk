/* memcpy overlaps and memmove handles it:
 *
 * - https://cs50.stackexchange.com/a/14729
 * - https://flak.tedunangst.com/post/memcpy-vs-memmove
 * - http://www.equestionanswers.com/c/memcpy-vs-memmove.php
 */

#include <stdio.h>
#include <string.h>

int main(void)
{
	int a = 10;
	char *s = "test";

	char buf[100];
	/* offset di sini berguna untuk menentukan indeks data yang akan dimasukkan */
	int offset = 0;

	memcpy(buf + offset, &a, sizeof(int));
	offset += sizeof(int);
	for (int i = 0; i < offset; i++)
		printf("%d ", buf[i]);
	printf("\n");
	printf("offset = %d\n", offset);  // 4, berarti jika menggunakan memcpy lagi, kita mulai dari indeks ke-4
	
	memcpy(buf + offset, s, strlen(s));
	offset += strlen(s);
	for (int i = 0; i < offset; i++)
		printf("%d ", buf[i]);
	printf("\n");
	printf("offset = %d\n", offset);
	
	return 0;
}
