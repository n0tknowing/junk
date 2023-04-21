/* A more complex example of pointer aliasing */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct boo {
	int a;
	int b;
	int c;
};

struct bar {
	char z;
};

/* without pointer
struct foo {
	struct boo d;
	struct bar e;
	int f;
};
*/

struct foo {
	struct boo *d;
	struct bar *e;
	int f;
};

static int foo_init(struct foo *this)
{
	struct boo *b_info = this->d; // without pointer, &this->d;
	struct bar *bar_info = this->e; // without pointer, &this->e;

	b_info->a = 0;
	b_info->b = 0;
	b_info->c = 0;

	bar_info->z = 'a';

	this->f = 0;
	return 0;
}

int main(void)
{
	struct foo *this = malloc(sizeof(*this));
	assert(this);

	/* { without pointer, remove all of these ...*/
	this->d = malloc(sizeof(struct boo));
	assert(this->d);

	this->e = malloc(sizeof(struct bar));
	assert(this->e);
	/* ... } */

	int ret = foo_init(this);
	assert(ret == 0);

	printf("this->f = %d\n", this->f);

	printf("this->boo->a = %d\n", this->d->a);
	printf("this->boo->b = %d\n", this->d->b);
	printf("this->boo->c = %d\n", this->d->c);

	printf("this->bar->z = %c\n", this->e->z);

	free(this->e); // without pointer, remove this
	free(this->d); // without pointer, remove this
	free(this);
	return 0;
}
