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
 int addrC = 0;
 int headerCount = 0;
bool comment = false;
struct Header {
	   int type; /* 0: col, 1: var, 2:const */
	  char name[40];
	   int cfa;
	   int value;
	  bool immediate;
	struct Header *next;
};
struct Header *header = NULL;

int preOptimizer(FILE* fIn) {
	return RC_OK;
}

int postOptimizer(FILE* fout) {
	return RC_OK;
}

void doHeader(struct Header** head_ref, char *name, int type, bool immediate) {
	struct Header* new_header = (struct Header*) malloc(sizeof(struct Header));
	strcpy(new_header->name, name);
	     new_header->type = type;
	      new_header->cfa = addrC;
	new_header->immediate = immediate;
	     new_header->next = (*head_ref);
	          (*head_ref) = new_header;
}

struct Header* searchHeader(struct Header *header, char *name) {
	while (header != NULL) {
		if (!strcmp(header->name, name))
			return header;
		header = header->next;
	}
	return NULL;
}

void listHeaders(struct Header *node) {
	char typeHeader[][10] = {"colonDef", "variable", "constant"};
	printf("\nHeaders:\n");
	while (node != NULL) {
		printf("[%s]\n", node->name);
		printf("     type: %s\n", typeHeader[node->type]);
		printf("      cfa: %04x\n", node->cfa);
		printf("    immed: %s\n\n", node->immediate ? "true" : "false");
		node = node->next;
	}
}

int searchBwords(char *word) {
	for (int n = 0; n < bW; n++) {
		if (!strcmp(bWords[n], word))
			return RC_OK;
	}
	return RC_ERROR;
}

int doCol(char *compiledLine) {
	doStatus = 1;

	for (int n = 0; n <= words; n++) {
		addrC++;
		if (!strcmp(lineSplited[n], ")")) comment = false;
		if (!strcmp(lineSplited[n], "(")) comment = true;
		if (comment) continue;
		if (!strcmp(lineSplited[n], "\\")) return RC_OK;

		if (!strcmp(lineSplited[n], ":")) {
			doHeader(&header, lineSplited[++n], 0, false);
			addrC--;
			sprintf(compiledLine, ".comment doCol %s\n",lineSplited[n]);
			continue;
		}
		if (!strcmp(lineSplited[n], ";")) {
			strcat(compiledLine, "exit\n");
			doStatus = 0;
			continue;
		}
		if (!strcmp(lineSplited[n], "variable")) {
			doHeader(&header, lineSplited[++n], 1, false);
			sprintf(compiledLine, "%s.comment doVar %s\ntop\n", compiledLine, lineSplited[n]);
			continue;
		}
		if (!strcmp(lineSplited[n], "constant")) {
			doHeader(&header, lineSplited[++n], 2, false);
			sprintf(compiledLine, "%s.comment doConst %s\nlit %04x\nput d-1\ntop d-1\n",compiledLine, lineSplited[n], addrC);
			continue;
		}
		if (!strcmp(lineSplited[n], "'")) {
			struct Header *search = searchHeader(header, lineSplited[++n]);
			if (search != NULL) {
				sprintf(compiledLine, "lit %04x\n", search->cfa);
				continue;
			}
			return RC_ERROR;
		}
		if (searchBwords(lineSplited[n]) == RC_OK) {
			strcat(compiledLine, lineSplited[n]);
			strcat(compiledLine, "\n");
			continue;
		}
		struct Header *search = searchHeader(header, lineSplited[n]);
		if (search != NULL) {
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
					strcat(compiledLine, "lit ");
					strcat(compiledLine, cfaStr);
					strcat(compiledLine, "\nget\n");
					continue;
				}
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
			char literal[9];
			if (res < 0x8000) {
				sprintf(literal, "%04x", (short)res);
				strcat(compiledLine, "lit ");
				strcat(compiledLine, literal);
				strcat(compiledLine, "\n");
				continue;
			} else {
				sprintf(literal, "%04x", ~(short)res);
				strcat(compiledLine, "lit ");
				strcat(compiledLine, literal);
				strcat(compiledLine, "\nneg\n");
				addrC++;
				continue;
			}
		} else
			return RC_ERROR;
	}
	return RC_OK;
}

/////////////////////////////////////////////////////////////////////////////////////

int sm1_compileLine(char* line, char* compiledLine) {
	words = getWords(line, lineSplited);

	if ((doStatus == 1) || (!strcmp(lineSplited[0], ":"))
			|| (!strcmp(lineSplited[0], ";"))) {
		doCol(compiledLine);
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
	remove(fileOut);

	if ((fWords = fopen(baseWords, "r")) == NULL) {
		perror("Error: can't open basewords-file");
		return RC_ERROR;
	}
	printf("\n--- start base words %s\n", baseWords);
	while (fgets(buf, sizeof(buf), fWords) != NULL) {
		getWords(buf, lineSplited);
		if ((!strcmp(strlwr(lineSplited[0]), ".word"))
				|| (!strcmp(strlwr(lineSplited[0]), ".macro"))) {
			strcpy(bWords[bW++], lineSplited[1]);
			printf("%s %s\n", lineSplited[0], lineSplited[1]);
		}
	}
	fclose(fWords);
	printf("--- end base words %s\n", baseWords);
	if ((fIn = fopen(fileIn, "r")) == NULL) {
		perror("Error: can't open source-file");
		return RC_ERROR;
	}

	if ((fOut = fopen(fileOut, "a")) == NULL) {
		perror("Error: can't open destination-file");
		return RC_ERROR;
	}

	printf("\n--- start compiling %s\n", fileIn);
	int cntLine = 0;
	fprintf(fOut, "jmp 0000\n");
	while (fgets(buf, sizeof(buf), fIn) != NULL) {
		buf[strlen(buf) - 1] = '\0';
		if (strcmp(buf, "") != 0) {
			printf("%03d %s\n", ++cntLine, buf);
			if (sm1_compileLine(buf, compiledLine) != RC_OK) {
				printf("Compile Error\n");
				return RC_ERROR;
			}
			fprintf(fOut, "%s", compiledLine);
			//printf("compiled\n%s", compiledLine);
			strcpy(compiledLine, "");
		}
	}
	struct Header *search = searchHeader(header, "main");
	if (search == NULL) {
		printf("Compile Error: not main\n");
		return RC_ERROR;
	}
	printf("--- end compiling %s\n", fileIn);
	listHeaders(header);
	fclose(fOut);
	fOut = fopen(fileOut, "r+");
	fprintf(fOut, "jmp %04x", search->cfa);
	fclose(fIn);
	fclose(fOut);

	return RC_OK;
}

#endif /* SM1_COMPILER_H_ */
