#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *
 * struct bencode_dict {
 *	char *key;
 *	void *value;
 *	struct bencode_dict *next;
 * };
 *
 * struct bencode_list {
 * 	struct bencode_data data;
 *	struct bencode_list *next;
 * };
 *
 * struct bencode_data {
 *	char *sval;
 *	long long ival;
 *	struct bencode_dict *dval;
 *	struct bencode_list *lval;
 * };
 *
 * struct bencode_ctx {
 *	struct bencode_data read;
 *	struct bencode_data write;
 * };
 *
 * struct bencode_ctx *bencode_init(void);
 * void bencode_end(struct bencode_ctx *);
 *
 * -----------------------------------------------------------------------
 *   char *bencode_read_str(struct bencode_ctx *, long long *);
 *   int bencode_write_str(struct bencode_ctx *, char *, long long);
 * -----------------------------------------------------------------------
 *
 * -----------------------------------------------------------------------
 *   long long bencode_read_int(struct bencode_ctx *);
 *   int bencode_write_int(struct bencode_ctx *, long long);
 * -----------------------------------------------------------------------
 *
 * -----------------------------------------------------------------------
 *   struct bencode_list *bencode_read_list(struct bencode_ctx *);
 *   int bencode_write_list(struct bencode_ctx *, struct bencode_list *);
 *
 *   struct bencode_list *list_add(struct bencode_list *, void *);
 *   struct bencode_list *list_del(struct bencode_list *);
 * -----------------------------------------------------------------------
 *
 * -----------------------------------------------------------------------
 *   struct bencode_dict *bencode_read_dict(struct bencode_ctx *);
 *   int bencode_write_dict(struct bencode_ctx *, struct bencode_dict *);
 *
 *   struct bencode_dict *dict_add(struct bencode_dict *, void *, void *);
 *   struct bencode_dict *dict_del(struct bencode_dict *, void *);
 * -----------------------------------------------------------------------
 *
 */

static long long parse_int(const void *data)
{
	int i, sign = 1;
	long long result = 0;
	const char *d = data;

	if (d[0] == '-') {
		sign = -1;
		d++;
	}

	for (i = 0; d[i]; i++) {
		if (isdigit(d[i])) {
			/* handle underflow/overflow? */
			result *= 10;
			result += (d[i] - '0') * sign;
		}
	}

	return result;
}

char *bencode_read_str(const void *data, long long *len)
{
	long long length;
	char *result, *temp;
	const char *d = data;

	if (!d || !isdigit(d[0]))
		return NULL;

	temp = strchr(d, ':');
	if (!temp)
		return NULL;

	temp++;

	length = parse_int(d);
	if (length < 0 || (size_t)length > strlen(temp))
		return NULL;

	result = malloc((size_t)length + 1);
	if (!result)
		return NULL;

	memcpy(result, temp, length);
	result[length] = '\0';

	if (len)
		*len = length;
	return result;
}

int bencode_write_str(const char *val, const long long len)
{
	if (len < 0)
		return -1;

	if (len > (long long)strlen(val))
		return -1;

	printf("%lld:%.*s\n", len, (int)len, val);
	return 0;
}

long long bencode_read_int(const void *data)
{
	long long result;
	const char *d = data;

	if (!d || d[0] != 'i' || d[strlen(d) - 1] != 'e')
		return 0;

	d++;

	result = parse_int(d);

	return result;
}

int bencode_write_int(const long long val)
{
	printf("i%llde\n", val);
	return 0;
}

static void bencode_print(const void *data)
{
	long long ires;
	char *sres = NULL;
	const char *d = data;

	char first = d[0];
	char last = d[strlen(d) - 1];

	switch (first) {
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		sres = bencode_read_str(d, &ires);
		if (sres) {
			printf("a string %s with length %lld\n", sres, ires);
			free(sres);
			sres = NULL;
		}
		break;
	case 'i':
		ires = bencode_read_int(d);
		printf("an integer with value %lld\n", ires);
		break;
	case 'd':
		if (last == 'e')
			printf("a fucking dictionary, but no parser yet\n");
		break;
	case 'l':
		if (last == 'e')
			printf("a fucking list, but no parser yet\n");
		break;
	default:
		break;
	}
}

int main(int argc, char **argv)
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s bencoded data...\n", argv[0]);
		return 1;
	}

	int i;
	const char *str;

	for (i = 1; i < argc; i++) {
		str = argv[i];
		bencode_print(str);
	}

	bencode_write_str("a string", 6);

	return 0;
}
