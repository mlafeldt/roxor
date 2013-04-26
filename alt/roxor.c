/*
 * Copyright (C) 2013 Mathias Lafeldt <mathias.lafeldt@gmail.com>
 *
 * This file is part of roxor.
 *
 * roxor is licensed under the terms of the MIT License. See LICENSE file.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>	/* isprint() */

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

/*
 * The XOR operator is vulnerable to a known-plaintext attack, since
 * plaintext XOR ciphertext = key.
 */
static void attack_cipher(const uint8_t *ciphertext, size_t ciphertext_len,
		const uint8_t *crib, size_t crib_len)
{
	size_t i, j;
	uint8_t key = 0, old_key;

	for (i = 0; i < ciphertext_len; i++) {
		old_key = ciphertext[i] ^ crib[0];
		j = 1;

		while ((j < crib_len) && ((i + j) < ciphertext_len) &&
				(key = ciphertext[i + j] ^ crib[j]) == old_key) {
			old_key = key;
			j++;
		}

		if (j == crib_len) {
			printf("Found text at 0x%zx (XOR key 0x%02x)\n", i, key);
			printf("  preview: ");

			for (j = 0; (j < 50) && ((i + j) < ciphertext_len); j++) {
				uint8_t ch = ciphertext[i + j] ^ key;
				if (!isprint(ch))
					ch = '.';
				printf("%c", ch);
			}

			printf("\n");
		}
	}
}

int main(int argc, char *argv[])
{
	char *filename = NULL, *crib = NULL;
	uint8_t *ciphertext = NULL;
	size_t ciphertext_len, crib_len;

	if (argc < 3) {
		fprintf(stderr, "usage: roxor <file> <crib>\n");
		return 1;
	}

	filename = argv[1];
	crib = argv[2];
	crib_len = strlen(crib);

	if (crib_len < 2) {
		fprintf(stderr, "error: crib too short\n");
		return 1;
	}

	if (read_file(&ciphertext, &ciphertext_len, filename)) {
		fprintf(stderr, "%s: read error\n", filename);
		return 1;
	}

	if (ciphertext_len < crib_len) {
		fprintf(stderr, "error: ciphertext too short\n");
		free(ciphertext);
		return 1;
	}

	attack_cipher(ciphertext, ciphertext_len, (uint8_t*)crib, crib_len);
	free(ciphertext);
	return 0;
}
