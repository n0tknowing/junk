#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/* convert virtual memory address to physical memory address */
static size_t vmem_to_phys(size_t vmem_addr)
{
	int fd;
	size_t phys_addr, page_frame_num;

	if ((fd = open("/proc/self/pagemap", O_RDONLY)) < 0)
		err(1, "open");

	if (pread(fd, &phys_addr, 8, (vmem_addr / 0x1000) << 3) != 8)
		err(1, "pread");

	page_frame_num = phys_addr & ((1ULL << 54) - 1);
	if (page_frame_num == 0) {
		errno = EPERM;
		err(1, "page_frame_num");
	}

	return page_frame_num * 0x1000 + vmem_addr % 0x1000;
}

int main(int argc, char **argv)
{
	if (argc < 2)
		return 1;

	size_t n = (size_t)((void *)argv[1]);
	size_t get = vmem_to_phys(n);

	printf("String: %s\n", argv[1]);
	printf("  Virtual address  : %016lx\n", n);
	printf("  Physical address : %016lx\n", get);

	return 0;
}
