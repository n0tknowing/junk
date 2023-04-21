/* compile with -lrt */
#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define B  1000000000.0

static struct timespec time_start(void)
{
	struct timespec start;

	if (clock_gettime(CLOCK_REALTIME, &start) < 0)
		err(1, "clock_gettime start");

	return start;
}

static struct timespec time_end(void)
{
	struct timespec end;

	if (clock_gettime(CLOCK_REALTIME, &end) < 0)
		err(1, "clock_gettime end");

	return end;
}

static double time_exec(struct timespec start, struct timespec end)
{
	return (end.tv_sec - start.tv_sec) +
	       (end.tv_nsec - start.tv_nsec) / B;
}

int main(void)
{
	double exec_time;
	unsigned char buff[512];
	struct timespec start, end;

	start = time_start();
	{
		FILE *fp = fopen("/dev/zero", "rb");
		if (!fp)
			goto out;
		fread(buff, 1, sizeof(buff), fp);
		fclose(fp);
	}
out:
	end = time_end();
	exec_time = time_exec(start, end);
	printf("fopen(3) + fread(3) Time = %f\n", exec_time);

	/*  -------------------------------------------------------  */

	start = time_start();
	{
		int fd = open("/dev/zero", O_RDONLY);
		if (fd < 0)
			goto fdout;
		read(fd, buff, sizeof(buff));
		close(fd);
	}
fdout:
	end = time_end();
	exec_time = time_exec(start, end);
	printf("open(2) + read(2) Time = %f\n", exec_time);

	return 0;
}
