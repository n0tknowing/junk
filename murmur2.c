#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static uint64_t murmurhash(const char *key)
{
	static const uint64_t m = 0xc6a4a7935bd1e995UL;
	const size_t len = strlen(key);
	const uint8_t *p = (const uint8_t *)key;
	const uint8_t *end = p + (len & ~0x7);
	uint64_t h = (len * m);

	while (p != end) {
		uint64_t k;
		memcpy(&k, p, sizeof(k));

		k *= m;
		k ^= k >> 0x2fUL;
		k *= m;

		h ^= k;
		h *= m;
		p += 0x8;
	}

	switch (len & 0x7) {
	case 7: h ^= (uint64_t)p[6] << 0x30UL; /* fall through */
	case 6: h ^= (uint64_t)p[5] << 0x28UL; /* fall through */
	case 5: h ^= (uint64_t)p[4] << 0x20UL; /* fall through */
	case 4: h ^= (uint64_t)p[3] << 0x18UL; /* fall through */
	case 3: h ^= (uint64_t)p[2] << 0x10UL; /* fall through */
	case 2: h ^= (uint64_t)p[1] << 0x8UL;  /* fall through */
	case 1: h ^= (uint64_t)p[0];           /* fall through */
		h *= m;
	default:
		break;
	};

	h ^= h >> 0x2fUL;
	h *= m;
	h ^= h >> 0x2fUL;

	return h;
}

int main(int argc, char **argv)
{
	const char *k;
	int i;
	uint64_t r;

	for (i = 1; i < argc; i++) {
		k = argv[i];
		r = murmurhash(k);
		dprintf(2, "key:        %s\n", k);
		dprintf(2, ">>  hash:   0x%016lx\n", r);
	}

	return 0;
}
