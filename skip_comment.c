#include <errno.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "usage: %s file\n", argv[0]);
		return 1;
	}

	FILE *f = fopen(argv[1], "r");
	if (!f) {
		perror("cannot open file");
		return 1;
	}

	int c;
	/* fgetc(file) = baca karakter selanjutnya yang ada di dalam file, dan
	 * mengembalikannya sebagai return value.
	 * disini, kita baca f terus menerus selama bukan EOF (end of file) */
	while ((c = fgetc(f)) != EOF) {
		/* jika ketemu '#' */
		if (c == '#') {
			/* baca lagi karakter selanjutnya, dan baca terus
			 * selama bukan '\n' */
			while ((c = fgetc(f)) != '\n') {
				/* jika selama kita meng-skip comment dan
				 * ketemu EOF, kita tutup saja file-nya
				 * menggunakan fclose karena kita sudah
				 * selesai membaca semua isi file */
				if (c == EOF) {
					fclose(f);
					return 0;
				}
			}
		} else { /* jika tidak */
			putchar(c); /* tampilkan saja karakternya */
		}
	}

	/* selesai, dan tutup filenya */
	fclose(f);
	return 0;
}
