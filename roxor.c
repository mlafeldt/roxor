#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int find_xor_byte(const unsigned char *buf, long bufsize, const char *passwd)
{
	int i, j, pwlen, hits = 0;
	unsigned char key = 0, oldkey, ch;

	pwlen = strlen(passwd);

	printf("Searching for XOR-encrypted password '%s' (%d bytes) ...\n", passwd, pwlen);

	for (i = 0; i < bufsize; i++) {
		oldkey = buf[i] ^ passwd[0];
		j = 1;
		while ((j < pwlen) && ((i+j) < bufsize) && (key = buf[i+j] ^ passwd[j]) == oldkey) {
			oldkey = key;
			j++;
		}
		if (j == pwlen) {
			printf("Found password at 0x%08X, key = 0x%02X\n", i, key);
			printf("Decrypted preview:\n");
			for (j = 0; (j < 50) && ((i+j) < bufsize); j++) {
				ch = buf[i+j] ^ key;
				if (!isprint(ch))
					ch = '.';
				printf("%c", ch);
			}
			printf("\n");
			hits++;
		}
	}

	return hits;
}

int main(int argc, char *argv[])
{
	FILE *fp;
	unsigned char *buf;
	long filesize;

	if (argc < 3) {
		return -1;
	}

	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: Can't open file '%s' for reading\n", argv[1]);
		return -2;
	}
	printf("File name = %s\n", argv[1]);

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	if (!filesize) {
		fclose(fp);
		return -3;
	}
	printf("File size = %lu bytes\n", filesize);

	buf = malloc(filesize);
	if (buf == NULL) {
		fclose(fp);
		return -4;
	}

	fseek(fp, 0, SEEK_SET);
	if (fread(buf, filesize, 1, fp) != 1) {
		fclose(fp);
		return -5;
	}
	fclose(fp);

	find_xor_byte(buf, filesize, argv[2]);

	free(buf);
	printf("Done!\n");

	return 0;
}
