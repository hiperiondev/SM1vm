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
#include <stdbool.h>

#define UNDER_OVER
//#define CARRY
#define EXTRAREGS
//#define EXTBITS

/////////////////////////////////////////////////////////////////////////////////////

char lineSplited[40][80];
char bWords[100][20];
int words;
int bW = 0;
int doStatus = 0;
int addr;
int headerCount = 0;
struct Header {
	uint8_t type; /* 0: col, 1: var, 2:const */
	char name[40];
	uint16_t cfa;
	uint16_t value;
	bool inmediate;
	struct Header *next;
};

int preOptimizer(FILE* fIn) {
	return RC_OK;
}

int postOptimizer(FILE* fout) {
	return RC_OK;
}

void doHeader(struct Header** head_ref, char *name, uint8_t type, bool inmediate) {
	struct Header* new_header = (struct Header*) malloc(sizeof(struct Header));
	new_header->type      = type;
	new_header->cfa       = addr;
	new_header->inmediate = inmediate;
	new_header->next      = (*head_ref);
	(*head_ref)           = new_header;
}

struct Header* searchHeader(struct Header *header, char *name) {
	while (header != NULL) {
		if (!strcmp(header->name, name))
			return header;
		header = header->next;
	}
	return 0;
}

int searchBwords(char *word) {
	for (int n = 0; n < bW; n++) {
		if (!strcmp(bWords[n], word))
			return RC_OK;
	}
	return RC_ERROR;
}

int doCol(char *compiledLine, struct Header *header) {
	doStatus = 1;

	for (int n = 0; n <= words; n++) {
		addr++;
		if (!strcmp(lineSplited[n], ":")) {
			doHeader(&header, lineSplited[++n], 0, false);
			addr--;
			continue;
		}
		if (!strcmp(lineSplited[n], ";")) {
			strcat(compiledLine, "exit\n");
			doStatus = 0;
			break;
		}
		if (searchBwords(lineSplited[n]) == RC_OK) {
			strcat(compiledLine, lineSplited[n]);
			strcat(compiledLine, "\n");
			continue;
		}
		struct Header *search = searchHeader(header, lineSplited[n]);
		if (search->cfa) {
			char cfaStr[5];
			sprintf(cfaStr, "%04x", search->cfa);
			if (search->type == 0) { // colon definition
				strcat(compiledLine, "cll ");
				strcat(compiledLine, cfaStr);
				strcat(compiledLine, "\n");
				continue;
			}
			if (search->type == 1) { // variable
				strcat(compiledLine, "lit ");
				strcat(compiledLine, cfaStr);
				strcat(compiledLine, "\n");
				continue;
			}
			if (search->type == 2) { // constant
				sprintf(cfaStr, "%04x", search->value);
				strcat(compiledLine, "lit ");
				strcat(compiledLine, cfaStr);
				strcat(compiledLine, "\n");
				continue;
			}
		}
		char *ptr; // literal
		bool lit = false;
		long res = strtol(lineSplited[n], &ptr, 10);
		if (strcmp(ptr, lineSplited[n]))
			lit = true;
		else {
			res = strtol(lineSplited[n], &ptr, 16);
			if (strcmp(ptr, lineSplited[n]))
				lit = true;
		}
		if (lit) {
			if (res > 0xffff) {
				printf("ERROR: literal too long\n");
				return RC_ERROR;
			}
			char literal[5];
			if (res < 0x7fff) {
				sprintf(literal, "%04x", res);
				strcat(compiledLine, "lit ");
				strcat(compiledLine, literal);
				strcat(compiledLine, "\n");
			} else {
				sprintf(literal, "%04x", res^0xffff);
				strcat(compiledLine, "lit ");
				strcat(compiledLine, literal);
				strcat(compiledLine, "\nneg\n");
			}
		}
		return RC_ERROR;
	}

	return RC_OK;
}

int doVar() {
	return RC_OK;
}

int doConst() {
	return RC_OK;
}

/////////////////////////////////////////////////////////////////////////////////////

int sm1_compileLine(char* line, char* compiledLine, struct Header *header) {
	words = getWords(line, lineSplited);

	if ((doStatus == 1) || (!strcmp(lineSplited[0], ":"))
			|| (!strcmp(lineSplited[0], ";"))) {
		doCol(compiledLine, header);
		return RC_OK;
	}

	return RC_OK;
}

int sm1_compileFile(char* fileIn, char* fileOut, char* baseWords) {
	FILE* fIn;
	FILE* fOut;
	FILE* fWords;
	char buf[80];
	char compiledLine[500];
	struct Header *header = NULL;
	remove(fileOut);

	if ((fWords = fopen(baseWords, "r")) == NULL) {
		perror("Error: can't open basewords-file");
		return RC_ERROR;
	}
	while (fgets(buf, sizeof(buf), fWords) != NULL) {
		getWords(buf, lineSplited);
		if ((!strcmp(strlwr(lineSplited[0]), ".word"))
				|| (!strcmp(strlwr(lineSplited[0]), ".macro"))) {
			strcpy(bWords[bW++], lineSplited[1]);
			printf("%s %s\n", lineSplited[0], lineSplited[1]);
		}
	}
	fclose(fWords);

	if ((fIn = fopen(fileIn, "r")) == NULL) {
		perror("Error: can't open source-file");
		return RC_ERROR;
	}

	if ((fOut = fopen(fileOut, "a")) == NULL) {
		perror("Error: can't open destination-file");
		return RC_ERROR;
	}

	while (fgets(buf, sizeof(buf), fIn) != NULL) {
		buf[strlen(buf) - 1] = '\0';
		if (strcmp(buf, "") != 0) {
			if (sm1_compileLine(buf, compiledLine, header) != RC_OK) {
				printf("Compile Error\n");
				return RC_ERROR;
			}
		}
		fprintf(fOut, "%s", compiledLine);
	}
	fclose(fIn);

	return RC_OK;
}

#endif /* SM1_COMPILER_H_ */
