#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

typedef struct {
	uint8_t signature[27];
	uint8_t unknown0[37];
	uint8_t unused[22];
	uint8_t name[40];
	uint8_t unknown1[28];
	uint16_t songsize;
	uint8_t unknown2[274];
	uint16_t num_inst;
} __attribute__((packed)) rl2_main0;

typedef struct {
	uint8_t flags[3];
	uint8_t name[11];
} __attribute__((packed)) rl2_instrument;

typedef struct {
	uint8_t pad[28];
	uint8_t oplparams[28]; // TODO
} __attribute__((packed)) rl2_embedded_instrument;

int main(int argc, char** argv) {
	for(int f = 1; f < argc; f++) {
		int fd;
		uint8_t* m;
		struct stat s;
		assert((fd = open(argv[f], O_RDWR)) > 0);
		assert(!fstat(fd, &s));
		assert((m = mmap(0, s.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) != MAP_FAILED);

#define hm0 ((rl2_main0*)m)
		if(memcmp(m, ">>> VC2, (C) ADLIB INC <<<\x0E", 27)) {
			printf("%s not an rl2\n", argv[f]);
			goto cln;
		}

		printf("song name: %.40s\n", hm0->name);
		printf("number of instruments: %hu\n", hm0->num_inst);
		rl2_instrument* hminst = m + sizeof(rl2_main0);
		int num_einst = hm0->num_inst;
		for(int i = 0; i < hm0->num_inst; i++) {
			printf("instrument %02i name: %.11s (flags %02X%02X%02X) ", i, hminst[i].name, hminst[i].flags[0],hminst[i].flags[1],hminst[i].flags[2]);
			if(hminst[i].flags[2] & 1) num_einst -= !!printf("(should have file)");
			putchar('\n');
		}
		printf("%Xh file instruments, %Xh embedded instruments (size: %u)\n", hm0->num_inst-num_einst, num_einst, num_einst*sizeof(rl2_instrument));
		rl2_embedded_instrument* hme = hminst + hm0->num_inst;
		for(int i = 0; i < num_einst; i++) {
			printf("embedded instrument %02i (parameters: ", i);
			for(int c = 0; c < 28; c++) printf("%02X ", hme[i].oplparams[c]);
			printf("\b)\n");
		}
		uint8_t* hmdat = hme + num_einst;
		printf("header was %u bytes, %u bytes follow (file says %hu)\n", s.st_size-hm0->songsize, s.st_size-(((long)hmdat)-((long)m)), hm0->songsize);

cln:		munmap(m, s.st_size);
		close(fd);
	}
}
