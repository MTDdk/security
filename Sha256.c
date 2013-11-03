/*
 * Sha256.c
 *
 *  Created on: 29/10/2013
 *      Author: MTD
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Length of output in bytes
 */
#define OUTPUT_SIZE 32
#define LENGTH_OF_INTEGER_IN_BITS 32
#define BYTE_POWER_OF_TWO 3
#define CHUNK_POWER_BITS 9
#define CHUNK_POWER_BYTES (CHUNK_POWER_BITS - BYTE_POWER_OF_TWO)
#define CHUNK_SIZE (1 << CHUNK_POWER_BYTES)//512 bits for chunk size = 64 bytes

const int HASH_VALUES[] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
const int ROUND_CONSTANTS[] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const char HEX_CHAR_TABLE[] = {
	'0', '1', '2', '3',
    '4', '5', '6', '7',
    '8', '9', 'a', 'b',
    'c', 'd', 'e', 'f'
};

unsigned int rotr(unsigned int word, int shift) {
	return ((word) >> shift) ^ ((word) << (LENGTH_OF_INTEGER_IN_BITS - shift) );
}

void digest(char* input, int inputLength, unsigned int* digest) {
	/*padding*/
	//calculate size
	int paddingSize = (((inputLength+CHUNK_SIZE+8) >> CHUNK_POWER_BYTES) << CHUNK_POWER_BYTES);
	char * padded = calloc(paddingSize, sizeof(char));

	//insert original input into the padded array
	int index;
	for (index = 0; index < inputLength; index++)
		padded[index] = input[index];
	/*for (index = inputLength; index < paddingSize; index++)
		padded[index] = 0;*/

	//the padding shall always start with a single '1' bit
	padded[inputLength] = 0x80;

	//inserting the length of the original input - 64 bits are reserved for this
	long orgLength = ((long)inputLength) << BYTE_POWER_OF_TWO;//length in bits
	int pSize = paddingSize;
	padded[--pSize] = (char) (orgLength & 0xFF);
	padded[--pSize] = (char) ((orgLength >>= 8)/* & 0xFF*/);
	padded[--pSize] = (char) ((orgLength >>= 8)/* & 0xFF*/);
	padded[--pSize] = (char) ((orgLength >>= 8)/* & 0xFF*/);
	padded[--pSize] = (char) ((orgLength >>= 8)/* & 0xFF*/);
	padded[--pSize] = (char) ((orgLength >>= 8)/* & 0xFF*/);
	padded[--pSize] = (char) ((orgLength >>= 8)/* & 0xFF*/);
	padded[--pSize] = (char) ((orgLength >>= 8)/* & 0xFF*/);

	//copy HASH_VALUES
	int H[8];
	for(index = 0; index < 8; index++)
		H[index] = HASH_VALUES[index];

	//process each chunk
	for (index = 0; index < paddingSize / CHUNK_SIZE; index++) {
		//copy the chunk into the 64 32-bits word
		unsigned int word[CHUNK_SIZE], w, c;
		for (w = 0, c = 0; w < 16; w++, c += 4)
			word[w] = ((padded[index * CHUNK_SIZE + c] & 0xFF) << 24) | ((padded[index * CHUNK_SIZE + c + 1] & 0xFF) << 16) | ((padded[index * CHUNK_SIZE + c + 2] & 0xFF) << 8) | (padded[index * CHUNK_SIZE + c + 3] & 0xFF);

		int s0, s1, ch ,maj, temp1, temp2;
		for (w = 16; w < 64; w++) {
			s0 = rotr(word[w-15], 7) ^ rotr(word[w-15], 18) ^ (word[w-15] >> 3);
			s1 = rotr(word[w-2], 17) ^ rotr(word[w-2], 19) ^ (word[w-2] >> 10);
			word[w] = word[w-16] + s0 + word[w-7] + s1;
		}

		//Initialize working variables to current hash value:
		int h[8];
		for(index = 0; index < 8; index++)
			h[index] = H[index];

		for (w = 0; w < 64; w++) {
			s1 = rotr(h[4], 6) ^ rotr(h[4], 11) ^ rotr(h[4], 25);
			ch = (h[4] & h[5]) ^ ((~h[4]) & h[6]);
			temp1 = h[7] + s1 + ch + ROUND_CONSTANTS[w] + word[w];
			s0 = rotr(h[0], 2) ^ rotr(h[0], 13) ^ rotr(h[0], 22);
			maj = (h[0] & h[1]) ^ (h[0] & h[2]) ^ (h[1] & h[2]);
			temp2 = s0 + maj;

			h[7] = h[6];
			h[6] = h[5];
			h[5] = h[4];
			h[4] = h[3] + temp1;
			h[3] = h[2];
			h[2] = h[1];
			h[1] = h[0];
			h[0] = temp1 + temp2;
		}

		H[0] += h[0];
		H[1] += h[1];
		H[2] += h[2];
		H[3] += h[3];
		H[4] += h[4];
		H[5] += h[5];
		H[6] += h[6];
		H[7] += h[7];
	}

	digest[0] = H[0];
	digest[1] = H[1];
	digest[2] = H[2];
	digest[3] = H[3];
	digest[4] = H[4];
	digest[5] = H[5];
	digest[6] = H[6];
	digest[7] = H[7];
}



void printHexChar(char c) {
	printf("%c", HEX_CHAR_TABLE[(c >> 4) & 0x0F]);
	printf("%c", HEX_CHAR_TABLE[c & 0x0F]);
}

void printHex(unsigned int * digest) {
	int i;
	for (i = 0; i < 8; i++) {
		printHexChar((char)(digest[i] >> 24));
		printHexChar((char)(digest[i] >> 16));
		printHexChar((char)(digest[i] >> 8));
		printHexChar((char)(digest[i] & 0xFF));
	}
	printf("\n");
}

int main(int argc, char **argv) {
	if (argc < 1)
		printf("State the input");

	//char input[50] = "The quick brown fox jumps over the lazy dog";
	unsigned int * output = calloc(OUTPUT_SIZE , sizeof(int));
	digest(argv[1], strlen(argv[1]), output);

	//printf("correct? %s\n\n", (-676791373 == output[0] ? "True" : "False"));

	printHex(output);

	free(output);

	return EXIT_SUCCESS;
}

