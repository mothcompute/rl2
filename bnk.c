#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

typedef struct {
	uint8_t magic0[3];
	uint8_t name[12];
	uint8_t magic1[5];
	uint16_t dptr;
	uint8_t magic2[6];
} __attribute__((packed)) goldopl;

int main(int argc, char** argv) {
	int fd;
	uint8_t* fm;
	struct stat s;
	int a = 0;
	for(int f = 1; f < argc; f++) {
		if((fd = open(argv[f], O_RDONLY)) >= 0) {
			fstat(fd, &s);
			fm = mmap(0, s.st_size, PROT_READ, MAP_SHARED, fd, 0);
			goldopl* map = fm + 48;
			int lp = 0x34B0;
			while(map->magic0[0] == 'O' && map->magic0[1] == '3' && map->magic0[2] == '_') {
				printf("%03u\t%.12s%c\t", a++, map->name, strlen(map->name) < 8 ? '\t' : ' ');
				//printf("\t%.12s", map->name);
				for(int i = 0; i < 28; i++) printf("%02X ", fm[map->dptr + i]);
				putchar('\n');
				lp = map->dptr;
				map++;
			}
			printf("\n%u instruments\n", a);
cln:			munmap(fm, s.st_size);
			close(fd);
		}
	}
}
