#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int read_file(uint8_t **buf, size_t *size, const char *path)
{
	FILE *fp;

	fp = fopen(path, "rb");
	if (fp == NULL)
		return -1;

	fseek(fp, 0, SEEK_END);
	*size = ftell(fp);

	*buf = malloc(*size);
	if (*buf == NULL) {
		fclose(fp);
		return -1;
	}

	fseek(fp, 0, SEEK_SET);
	if (fread(*buf, *size, 1, fp) != 1) {
		fclose(fp);
		return -1;
	}

	fclose(fp);
	return 0;
}

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
	char *filename = NULL, *crib = NULL;
	uint8_t *ciphertext = NULL;
	size_t ciphertext_len;

	if (argc < 3) {
		fprintf(stderr, "usage: roxor <file> <crib>\n");
		return 1;
	}

	filename = argv[1];
	crib = argv[2];

	if (read_file(&ciphertext, &ciphertext_len, filename)) {
		fprintf(stderr, "%s: read error\n", filename);
		return 1;
	}

	printf("File name = %s\n", filename);
	printf("File size = %lu bytes\n", ciphertext_len);

	find_xor_byte(ciphertext, ciphertext_len, crib);

	free(ciphertext);

	printf("Done!\n");

	return 0;
}
