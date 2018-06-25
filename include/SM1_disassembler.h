/*
 * SM1_disassembler.h
 *
 *  License: GNU GPL V3
 *
 *  Created on: 15 jun. 2018
 *  Author: Emiliano Augusto Gonzalez (egonzalez.hiperion@gmail.com)
 */

#define UNDER_OVER
//#define CARRY
#define EXTRAREGS
//#define EXTBITS

#include "SM1.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define EP(x) [x] = #x  /* enum print */

/////////////////////////////////////////////////////////////////////////////////////

char dis[100];

char *strlwr(char *str) {
	unsigned char *p = (unsigned char *) str;
	while (*p) {
		*p = tolower((unsigned char) *p);
		p++;
	}
	return str;
}

char * removeSubStr(char *str, const char *substr) {
	size_t m1 = strlen(str);
	size_t m2 = strlen(substr);
	if (!(m1 < m2)) {
		for (char *p = str; (p = strstr(p, substr)) != NULL;) {
			size_t n = m1 - (p + m2 - str);
			memmove(p, p + m2, n + 1);
		}
	}
	return str;
}

const char* ALU[] = {
		EP(ALU_OP_TOP),
		EP(ALU_OP_SCN),
		EP(ALU_OP_TRS),
		EP(ALU_OP_GET),
		EP(ALU_OP_PUT),
		EP(ALU_OP_DPL),
		EP(ALU_OP_DML),
		EP(ALU_OP_AND),
		EP(ALU_OP_BOR),
		EP(ALU_OP_XOR),
		EP(ALU_OP_NEG),
		EP(ALU_OP_DEC),
		EP(ALU_OP_EQ0),
		EP(ALU_OP_EQU),
		EP(ALU_OP_UCP),
		EP(ALU_OP_CMP),
		EP(ALU_OP_RSH),
		EP(ALU_OP_LSH),
		EP(ALU_OP_GSP),
		EP(ALU_OP_GRS),
		EP(ALU_OP_SSP),
		EP(ALU_OP_SRP),
		EP(ALU_OP_GST),
		EP(ALU_OP_SST),
		EP(ALU_OP_SND),
		EP(ALU_OP_RCV),
		EP(ALU_OP_UMD),
		EP(ALU_OP_MOD),
		EP(ALU_OP_REG),
		EP(ALU_OP_SRG),
		EP(ALU_OP_NP2),
		EP(ALU_OP_BYE)
};

const char* OPTYPE[] = {
		EP(OP_JMP),
		EP(OP_JMZ),
		EP(OP_CLL),
		EP(OP_ALU),
		EP(OP_LIT)
};

const char* FLAGS[] = {
		EP(ALU_F_T2N),
		EP(ALU_F_T2R),
		EP(ALU_F_N2T),
		EP(ALU_F_R2P)
};

static const uint16_t delta[] = { 0, 1, -1, -2 };

char* sm1_disasembly(uint16_t word) {
	int _delta[] = { 0, 1, -1, -2 };
	char hex[6];
	int f = 0;
	strcpy(dis, "");

	if (word & OP_LIT) {
		sprintf(hex, "%04x", ARG_LIT(word));
		strcat(dis, OPTYPE[OP_LIT]);
		strcat(dis, " ");
		strcat(dis, hex);
	} else {
		switch (OP(word)) {
		case OP_JMZ:
			sprintf(hex, "%04x",  ARG_OP(word));
			strcat(dis, OPTYPE[OP_JMZ]);
			strcat(dis, " ");
			strcat(dis, hex);
			break;
		case OP_JMP:
			sprintf(hex, "%04x",  ARG_OP(word));
			strcat(dis, OPTYPE[OP_JMP]);
			strcat(dis, " ");
			strcat(dis, hex);
			break;
		case OP_CLL:
			sprintf(hex, "%04x",  ARG_OP(word));
			strcat(dis, OPTYPE[OP_CLL]);
			strcat(dis, " ");
			strcat(dis, hex);
			break;
		case OP_ALU:
			strcpy(dis, ALU[ALU_OP(word)]);
			strcat(dis, " (");
			if (word & ALU_F_R2P) {
				f =1;
				strcat(dis, FLAGS[ALU_F_R2P]);
			}
			if (word & ALU_F_T2N) {
				if (f==1) strcat(dis, "|");
				f = 1;
				strcat(dis, FLAGS[ALU_F_T2N]);
			}
			if (word & ALU_F_T2R) {
				if (f==1) strcat(dis, "|");
				f = 1;
				strcat(dis, FLAGS[ALU_F_T2R]);
			}
			if (word & ALU_F_N2T) {
				if (f==1) strcat(dis, "|");
				strcat(dis, FLAGS[ALU_F_N2T]);
			}

			strcat(dis, ") [dp:");
			sprintf(hex, "%*d|", 2, _delta[ALU_DS(word)]);
			strcat(dis, hex);
			strcat(dis, "rp:");
			sprintf(hex, "%*d", 2, _delta[ALU_RS(word)]);
			strcat(dis, hex);
			strcat(dis, "]");
		}
	}

	strcpy(dis,removeSubStr(dis,"ALU_"));
	strcpy(dis,removeSubStr(dis,"OP_"));
	strcpy(dis,removeSubStr(dis,"F_"));
	return strlwr(dis);
}
