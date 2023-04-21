#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct hstr {
	char *v;
	size_t l;
	uint64_t h;
} hstr;

/* a slightly modified djb2 hash.
 * original: http://www.cse.yorku.ca/~oz/hash.html
 */
static uint64_t __hash(unsigned char *str, size_t l)
{
	uint64_t res = 0x1505;

	while (l--)
		res = ((res << 5)) ^ *str++;

	return res;
}

hstr *__new(char *v)
{
	hstr *__t = malloc(sizeof(hstr));
	__t->v = strdup(v);
	__t->l = strlen(v);
	__t->h = __hash((unsigned char *)v, __t->l);
	return __t;
}

char *__val(hstr *__t)
{
	return __t->v;
}

int __cmp(hstr *__l, hstr *__r)
{
	return __l->h == __r->h;
}

void __done(hstr *__t)
{
	free(__t->v);
	__t->v = NULL;
	__t->l = 0;
	__t->h = 0;
	free(__t);
}

hstr *__mv(hstr *__t, char *v)
{
	__done(__t);
	__t = __new(v);
	return __t;
}

int main(void)
{
	hstr *s = __new("Hello world");
	hstr *p = __new("HELLO WORLD");

	printf("s = %s\n", __val(s));
	printf("p = %s\n", __val(p));

	if (__cmp(s, p))
		printf("OK\n");

	p = __mv(p, s->v);

	if (__cmp(s, p))
		printf("OK\n");

	__done(s);
	__done(p);
	return 0;
}
