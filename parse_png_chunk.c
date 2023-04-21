#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint32_t crc32_for_byte(uint32_t r) {
  for(int j = 0; j < 8; ++j)
    r = (r & 1? 0: (uint32_t)0xEDB88320L) ^ r >> 1;
  return r ^ (uint32_t)0xFF000000L;
}

/* Any unsigned integer type with at least 32 bits may be used as
 * accumulator type for fast crc32-calulation, but unsigned long is
 * probably the optimal choice for most systems. */
typedef unsigned long accum_t;

void init_tables(uint32_t* table, uint32_t* wtable) {
  for(size_t i = 0; i < 0x100; ++i)
    table[i] = crc32_for_byte(i);
  for(size_t k = 0; k < sizeof(accum_t); ++k)
    for(size_t w, i = 0; i < 0x100; ++i) {
      for(size_t j = w = 0; j < sizeof(accum_t); ++j)
	w = table[(uint8_t)(j == k? w ^ i: w)] ^ w >> 8;
      wtable[(k << 8) + i] = w ^ (k? wtable[0]: 0);
    }
}

uint32_t crc32(uint32_t crc, const void* data, size_t n_bytes) {
  static uint32_t table[0x100], wtable[0x100*sizeof(accum_t)];
  size_t n_accum = n_bytes/sizeof(accum_t);
  if(!*table)
    init_tables(table, wtable);
  for(size_t i = 0; i < n_accum; ++i) {
    accum_t a = crc ^ ((accum_t*)data)[i];
    for(size_t j = crc = 0; j < sizeof(accum_t); ++j)
      crc ^= wtable[(j << 8) + (uint8_t)(a >> 8*j)];
  }
  for(size_t i = n_accum*sizeof(accum_t); i < n_bytes; ++i)
    crc = table[(uint8_t)crc ^ ((uint8_t*)data)[i]] ^ crc >> 8;
  return crc;
}

static uint8_t is_png(FILE *fp)
{
	uint8_t png[8];
	if (fread(png, 1, 8, fp) != 8) {
		errno = EIO;
		return 0;
	}
	if (!memcmp(png, "\x89PNG\x0d\x0a\x1a\x0a", 8))
		return 1;
	return 0;
}

static uint32_t
read_chunk_length(FILE *fp)
{
	uint32_t ret = 0u;

	if (fread(&ret, 4, 1, fp) != 1) {
		errno = EIO;
		return 0;
	}

	ret = __builtin_bswap32(ret);
	if (ret > (INT_MAX - 1)) {
		errno = EOVERFLOW;
		return 0;
	}

	return ret;
}

static char *
read_chunk_type(FILE *fp, char *buf)
{
	if (fread(buf, 1, 4, fp) != 4) {
		errno = EIO;
		return NULL;
	}

	return buf;
}

static uint8_t *
read_chunk_data(FILE *fp, uint32_t len)
{
	uint8_t *buf = malloc(len);
	if (!buf) {
		errno = ENOMEM;
		return NULL;
	}

	if (fread(buf, 1, len, fp) != len) {
		free(buf);
		errno = EIO;
		return NULL;
	}

	return buf;
}

static uint32_t
check_chunk_crc(FILE *fp, uint8_t *data, uint32_t len, char *type)
{
	uint32_t chunk_crc, to_check;

	if (fread(&chunk_crc, 4, 1, fp) != 1) {
		errno = EIO;
		return 0;
	}

	chunk_crc = __builtin_bswap32(chunk_crc);
	to_check = crc32(0u, type, 4);
	to_check = crc32(to_check, data, len);

	return (chunk_crc == to_check) ? chunk_crc : 0u;
}

int main(int argc, char **argv)
{
	int iend = 0;
	uint32_t length = 0, crc = 0;
	char type[5] = {0};
	uint8_t *data = NULL;

	if (argc != 2) {
		fprintf(stderr, "usage: %s file.png\n", argv[0]);
		return 1;
	}

	FILE *fp = fopen(argv[1], "rb");
	if (!fp) {
		perror("fopen");
		return 1;
	}

	if (!is_png(fp)) {
		perror("is_png");
		goto err;
	}

	while (iend != 1) {
		length = read_chunk_length(fp);
		if (errno) {
			perror("read_chunk_length");
			goto err;
		}
		if (!read_chunk_type(fp, type)) {
			perror("read_chunk_type");
			goto err;
		}
		if (!strcmp(type, "IEND"))
			iend = 1;
		if (length > 0) {
			data = read_chunk_data(fp, length);
			if (!data)
				goto err;
		}
		
		crc = check_chunk_crc(fp, data, length, type);
		if (!crc) {
			free(data);
			goto err;
		}

		printf("[%s] -- length: %u, crc: %08x\n", type, length, crc);
		if (length > 0)
			free(data);
	}

err:
	fclose(fp);
	return 0;
}
