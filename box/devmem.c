#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <start address> <size>\n", argv[0]);
        return 1;
    }

    unsigned long start_addr = strtoul(argv[1], NULL, 0);
    unsigned long size = strtoul(argv[2], NULL, 0);

    int mem_fd = open("/dev/mem", O_RDONLY);
    if (mem_fd == -1) {
        perror("open");
        return 1;
    }

    void *map_base = mmap(NULL, size, PROT_READ, MAP_SHARED, mem_fd, start_addr);
    if (map_base == MAP_FAILED) {
        perror("mmap");
        close(mem_fd);
        return 1;
    }

    printf("Memory content at address 0x%lx:\n", start_addr);
    for (unsigned long i = 0; i < size; i++) {
        printf("%02x ", ((unsigned char *)map_base)[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");

    if (munmap(map_base, size) == -1) {
        perror("munmap");
        close(mem_fd);
        return 1;
    }

    close(mem_fd);
    return 0;
}
