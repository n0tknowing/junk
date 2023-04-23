/* setlocale() is simple but it's not good for multithreaded */
/*
 * updating locale on linux:
 * - edit /etc/locale.gen and uncomment (or comment to remove) locale you want
 * - after editing, run locale-gen
 * - next, update current locale with update-locale
 * - and last, check it with locale -a to make sure it works
 * */

#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <locale.h>
#include <stdio.h>

#define LC_NULL  (locale_t)0

int main(void)
{
	locale_t new;

	new = newlocale(LC_ALL_MASK, "", LC_NULL);
	if (new == LC_NULL) {
		perror("newlocale()");
		return 1;
	}

	uselocale(new);

	printf("%f\n", 12345.67);

	freelocale(new);
	return 0;
}
