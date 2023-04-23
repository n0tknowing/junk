#include <stdio.h>
#include <curl/curl.h>

/* no output */
static size_t cb(char *data, size_t n, size_t l, void *userp)
{
	(void)data;
	(void)userp;
	return n * l;
}

int main(int argc, char **argv)
{
	CURL *handler;
	CURLcode res = 1;
	long port = -1;
	const char *url, *ua;
	int i;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	handler = curl_easy_init();
	if (!handler)
		return 1;

	ua = "Whatever/1234.5678";

	for (i = 1; i < argc; i++) {
		url = argv[i];

		curl_easy_setopt(handler, CURLOPT_URL, url);
		curl_easy_setopt(handler, CURLOPT_USERAGENT, ua);
		curl_easy_setopt(handler, CURLOPT_WRITEFUNCTION, cb);
		curl_easy_setopt(handler, CURLOPT_TIMEOUT, 60L);

		res = curl_easy_perform(handler);
		if (res != CURLE_OK) {
			fprintf(stderr, "[%s] %s\n", url, curl_easy_strerror(res));
			continue;  /* get next URL */
		}

		res = curl_easy_getinfo(handler, CURLINFO_PRIMARY_PORT, &port);
		if (res == CURLE_OK)
			printf("[%s] Connected to port %ld\n", url, port);
		else
			fprintf(stderr, "[%s] Failed to retrieve port: %s\n", url,
					curl_easy_strerror(res));
	}

	curl_easy_cleanup(handler);
	curl_global_cleanup();
	return !!res;
}
