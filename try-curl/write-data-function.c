#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define UA "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36" \
	"(KHTML, like Gecko) Chrome/99.0.4844.74 Safari/537.36"

struct res {
	char *p;
	size_t size;
};

static size_t cb(void *data, size_t size, size_t n, void *userp)
{
	size_t prev = size * n;
	struct res *r = userp;
	char *p = realloc(r->p, r->size + prev + 1);
	if (!p)
		return 0;

	r->p = p;
	memcpy(r->p + r->size, data, prev);
	r->size += prev;
	r->p[r->size] = 0;

	return prev;
}

int main(int argc, char **argv)
{
	if (argc < 2)
		return 1;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	CURLcode r = 1;
	CURL *c = curl_easy_init();
	if (!c)
		goto g_clean;

	long protocols = CURLPROTO_HTTP | CURLPROTO_HTTPS;
	struct res *buf = malloc(sizeof(struct res));
	if (!buf)
		goto clean;

	buf->p = NULL;
	buf->size = 0;

	curl_easy_setopt(c, CURLOPT_URL, argv[1]);
	curl_easy_setopt(c, CURLOPT_USERAGENT, UA);
	curl_easy_setopt(c, CURLOPT_WRITEFUNCTION, cb);
	curl_easy_setopt(c, CURLOPT_WRITEDATA, (void *)buf);
	curl_easy_setopt(c, CURLOPT_PROTOCOLS, protocols);

	r = curl_easy_perform(c);
	if (r != CURLE_OK)
		goto clean;

	printf("Data: %s\n", buf->p ? buf->p : "None");
	printf("Total data: %zu\n", buf->size);

	free(buf->p);
	free(buf);

clean:
	if (r)
		fprintf(stderr, "%s (%d): %s\n", argv[1], r, curl_easy_strerror(r));

	curl_easy_cleanup(c);

g_clean:
	curl_global_cleanup();
	return !!r;
}
