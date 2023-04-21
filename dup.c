/*
 * kesimpulan:
 *
 * ketika kita memanggil dup() atau dup2(), fd baru akan tetap ada
 * meskipun fd lama sudah di-close().
 *
 * yang membedakan dup() dan dup2() adalah nomor dari fd baru.
 *
 * dup() mengembalikan fd baru dari nomor terendah dari yang tersedia.
 * misalnya, 0 1 2 sudah terbuka, dan 3 adalah fd lama, berarti jika panggil
 * dup() ada kemungkin fd barunya adalah nomor 4.
 *
 * sedangkan dup2() kita bisa memilih nomor fd dari argumen kedua dup2().
 * jika nomor fd yang dipilih sudah terbuka, maka dup2() secara diam-diam akan
 * memanggil close() lalu membukanya kembali.
 * */

#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
	int fd = open("dup.c", O_RDONLY), new;
	if (fd < 0)
		err(1, "open");

/*
	new = dup2(fd, 9);
	if (new 0) {
		close(fd);
		err(1, "dup2");
	}
*/
	new = dup(fd);
	if (new < 0) {
		close(fd);
		err(1, "dup");
	}

	printf("close(old) = %d\n", close(fd));
	while (1)
		sleep(1);

	return 0;
}
