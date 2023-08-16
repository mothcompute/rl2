#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

// theorized (datatypes may be different)
typedef struct {
	uint8_t magic[14];
	uint32_t fmt;
	uint32_t rate;
	uint32_t size;
	uint16_t zero[9];
} __attribute__((packed)) igoldsmp;

int main(int argc, char** argv) {
	int fd;
	igoldsmp* fm;
	struct stat s;
	int a = 0;
	for(int f = 1; f < argc; f++) {
		if((fd = open(argv[f], O_RDONLY)) >= 0) {
			fstat(fd, &s);
			fm = mmap(0, s.st_size, PROT_READ, MAP_SHARED, fd, 0);
			for(int i = 0; i < 9; i++) {a += fm->zero[i];}
			if(
				memcmp(fm, "GOLD SAMPLE \x01\x01", 14) ||
				a ||
				44 + fm->size < s.st_size
			) {
				printf("%s incomplete or invalid gold sample\n", argv[f]);
				goto cln;
			}
			printf(
				"gold sample %s\n"
				"format:\t%u bits\n"
				"rate:\t%uhz\n"
				"size:\t%u bytes (+44 header)\n",
				argv[f],
				8 << fm->fmt,
				fm->rate,
				fm->size
			);
cln:			munmap(fm, s.st_size);
			close(fd);
		}
	}
}
