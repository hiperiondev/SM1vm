/*
 * SM1_assembler.h
 *
 *  License: GNU GPL V3
 *
 *  Created on: 24 jun. 2018
 *  Author: Emiliano Augusto Gonzalez (egonzalez.hiperion@gmail.com)
 */

#include <stdio.h>
#include <string.h>
#include "jwHash.h"

#define UNDER_OVER
//#define CARRY
#define EXTRAREGS
//#define EXTBITS

/////////////////////////////////////////////////////////////////////////////////////

const uint8_t VMFLAGS_POS[]  = { ALU_F_T2N, ALU_F_T2R, ALU_F_N2T, ALU_F_R2P };
const uint8_t VMFLAGS_CODE[] = { 0x80, 0x40, 0x20, 0x10 };
int sm1_assembleFile(char* fileIn, char* fileOut);
uint16_t addr = 0x0000;
jwHashTable * equ   = NULL;
jwHashTable * word  = NULL;
jwHashTable * label = NULL;

int getWords(char *base, char target[10][20])
{
	int n=0,i,j=0;

	for(i=0;1;i++)
	{
		if(base[i]!=' '){
			target[n][j++]=base[i];
		}
		else{
			target[n][j++]='\0';
			n++;
			j=0;
		}
		if(base[i]=='\0')
		    break;
	}
	return n;

}

int opCmp(char *op, char *value){
	return strcmp(strlwr(op), value);
}

int directives(char* line, char* fileOut) {
	char lineSplited[10][20], str[40];
	int words = getWords(line, lineSplited);
	char * hresult;

	if (opCmp(lineSplited[0], ".equ") == 0) {      // assigns a value to a label.
		add_str_by_str(equ,lineSplited[1], lineSplited[2]);
		get_str_by_str(equ,lineSplited[1], &hresult);
		printf (".equ %s %s\n", lineSplited[1], hresult);
		return 0;
	}
	if (opCmp(lineSplited[0], ".macro") == 0) {    // start of a macro. takes the name as parameter.when the name of the macro
		printf (".macro %d\n",words);              // is written later in the program, the macro definition is expanded
		                                           // at the place it was used. A Macro can take up to 10 parameters.
		                                           // these parameters are referred to as @0-@9 within the Macro definition.
		return 0;
	}
	if (opCmp(lineSplited[0], ".endm") == 0) {     // the end of a macro definition.
		printf (".endm %d\n",words);
		return 0;
	}


	if (opCmp(lineSplited[0], ".include") == 0) {  // start reading from a specified file.
		printf (".include %s\n",lineSplited[1]);
		sm1_assembleFile(lineSplited[1], fileOut);
		printf (".endinclude\n");
		return 0;
	}
	if (opCmp(lineSplited[0], ".word") == 0) {     // define new mnemonic from complete line. Ex. dup@  get t2n d+1
		int ptr = 2;
		while (ptr <= words) {
			strcat(str, lineSplited[ptr]);
			strcat(str, " ");
			ptr++;
		}
		add_str_by_str(word,lineSplited[1], str);
		get_str_by_str(word,lineSplited[1], &hresult);
		printf (".word %s %s\n", lineSplited[1], hresult);
		return 0;
	}
	if (opCmp(lineSplited[0], ".label") == 0) {    // assign the address of label.
		sprintf(str,"%04x",addr);
		add_str_by_str(label,lineSplited[1], str);
		get_str_by_str(label,lineSplited[1], &hresult);
		printf (".label %s %s\n", lineSplited[1], hresult);
		return 0;
	}

	return 1;
}

//////////////////////////////////////////////////////////////

uint16_t sm1_assembleLine(char* line) {
	printf ("%s\n", line);
	int words, value, w;
	char lineSplited[10][20], str[20];
	char * hresult = NULL;

	words = getWords(line, lineSplited);
	value = (int) strtol(lineSplited[1], NULL, 16);

	get_str_by_str(word,lineSplited[0], &hresult);
	if (hresult != NULL) words = getWords(hresult, lineSplited);

	get_str_by_str(equ,lineSplited[1], &hresult);
	if (hresult == NULL) get_str_by_str(label,lineSplited[1], &hresult);
	if (hresult != NULL) value = (int) strtol (hresult, NULL, 16);;

	if (opCmp(lineSplited[0], "lit") == 0) {
		if (value < 32768)
			return (0x8000 | value);
		printf("ASSEMBLER ERROR: lit too long\n");
		exit(1);
	}

	if (opCmp(lineSplited[0], "jmp") == 0) {
		if (value < 8192)
			return (0x0000 | value);
		printf("ASSEMBLER ERROR: jmp too long\n");
		exit(1);
	}
	if (opCmp(lineSplited[0], "jmz") == 0) {
		if (value < 8192)
			return (0x2000 | value);
		printf("ASSEMBLER ERROR: jmz too long\n");
		exit(1);
	}
	if (opCmp(lineSplited[0], "cll") == 0) {
		if (value < 8192)
			return (0x4000 | value);
		printf("ASSEMBLER ERROR: cll too long\n");
		exit(1);
	}

	value = 0xffff;
	for (int w=0; w < 32; w++) {
		strcpy(str,ALU[w]);
        removePrefix(str);
		if (opCmp(str, lineSplited[0]) == 0) {
			value = w << 8;
			break;
		}
	}

	if (value == 0xffff) {
		printf("ASSEMBLER ERROR: unknown mnemonic\n");
		exit(1);
	}

	value |= 0x6000;

	int pos = 1, fl = 0;
	while (pos < words) {
		for (w=0; w < 4; w++) {
			strcpy(str, VMFLAGS[VMFLAGS_POS[w]]);
			removePrefix(str);
			if (opCmp(str, lineSplited[pos]) == 0) {
				value |= VMFLAGS_CODE[w];
				fl = 1;
				break;
			}
		}

		for (w=0; w < 6; w++) {
			strcpy(str, DELTA[w]);
			removePrefix(str);
			if (opCmp(str, lineSplited[pos]) == 0) {
				value |= DELTA_CODE[w];
				fl = 1;
				break;
			}
		}
		if (fl == 0) {
			printf("ASSEMBLER ERROR: unknown mnemonic\n");
			exit(1);
		}
		pos++;
		fl = 0;
	}

	return value;
}


int sm1_assembleFile(char* fileIn, char* fileOut) {
	FILE* fIn;
	FILE* fOut;
	char buf[80];
	if (equ == NULL) {
		equ   = create_hash(100);
		word  = create_hash(100);
		label = create_hash(100);
	}

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
			if (directives(buf, fileOut)) {
				printf("    %04x ", addr);
				fprintf(fOut, "%04x\n", sm1_assembleLine(buf));
				addr++;
			}
		}
	}
	fclose(fIn);

	return 0;
}
