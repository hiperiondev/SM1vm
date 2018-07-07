/*
 * SM1_compiler.h
 *
 *  License: GNU GPL V3
 *
 *  Created on: 3 Jul. 2018
 *  Author: Emiliano Augusto Gonzalez (egonzalez.hiperion@gmail.com)
 */

#ifndef SM1_COMPILER_H_
#define SM1_COMPILER_H_

#include "SM1.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "jwHash.h"

#define UNDER_OVER
//#define CARRY
#define EXTRAREGS
//#define EXTBITS

/////////////////////////////////////////////////////////////////////////////////////

int sm1_assembleFile(char* fileIn, char* fileOut);

uint16_t sm1_assembleLine(char* line);

uint16_t sm1_compileLine(char* line) {
	//char lineSplited[10][80], str[20];
	//int words = getWords(line, lineSplited);

	return RC_OK;
}

int sm1_compileFile(char* fileIn, char* fileOut, char* baseWords) {
	FILE* fIn;
	FILE* fOut;
	FILE* fWords;
	char buf[80];
	char lineSplited[40][80];
    char bWords[100][20];
	int bW=0;
	remove(fileOut);

	if ((fWords = fopen(baseWords, "r")) == NULL) {
		perror("Error: can't open basewords-file");
		return 1;
	}
	while (fgets(buf, sizeof(buf), fWords) != NULL) {
		if ((strcmp(strlwr(lineSplited[0]), ".word"))||(strcmp(strlwr(lineSplited[0]), ".macro")) == 0)
			strcpy(bWords[bW++],lineSplited[1]);
	}
	bW--;
	fclose(fWords);

	if ((fIn = fopen(fileIn, "r")) == NULL) {
		perror("Error: can't open source-file");
		return 1;
	}

	if ((fOut = fopen(fileOut, "a")) == NULL) {
		perror("Error: can't open destination-file");
		return 1;
	}

	while (fgets(buf, sizeof(buf), fIn) != NULL) {
		buf[strlen(buf) - 1] = '\0';
		if (strcmp(buf, "") != 0) {
			fprintf(fOut, "%04x\n", sm1_compileLine(buf));

		}
	}
	fclose(fIn);

	return 0;
}

#endif /* SM1_COMPILER_H_ */
