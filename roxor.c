#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static int read_file(uint8_t **buf, size_t *size, const char *path)
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

static int attack(const uint8_t *ciphertext, size_t ciphertext_len, const char *crib)
{
	size_t i, j, hits = 0;
	uint8_t key = 0, old_key;
	size_t crib_len = strlen(crib);

	printf("Searching for XOR-encrypted password '%s' (%zd bytes) ...\n", crib, crib_len);

	for (i = 0; i < ciphertext_len; i++) {
		old_key = ciphertext[i] ^ crib[0];
		j = 1;

		while ((j < crib_len) && ((i + j) < ciphertext_len) &&
				(key = ciphertext[i + j] ^ crib[j]) == old_key) {
			old_key = key;
			j++;
		}

		if (j == crib_len) {
			printf("Found password at 0x%08zX, key = 0x%02X\n", i, key);
			printf("Decrypted preview:\n");
			for (j = 0; (j < 50) && ((i+j) < ciphertext_len); j++) {
				char ch = ciphertext[i+j] ^ key;
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

	attack(ciphertext, ciphertext_len, crib);

	free(ciphertext);

	printf("Done!\n");

	return 0;
}
