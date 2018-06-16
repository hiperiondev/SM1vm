/*
 * SM1_disassembler.h
 *
 *  License: GNU GPL V3
 *
 *  Created on: 15 jun. 2018
 *  Author: Emiliano Augusto Gonzalez (egonzalez.hiperion@gmail.com)
 */


//#define DEBUG 1
#define UNDER_OVER
//#define CARRY
#define EXTRAREGS
//#define EXTBITS

#include "SM1.h"
#include <stdlib.h>
#include <stdio.h>

#define EP(x) [x] = #x  /* enum print */
/////////////////////////////////////////////////////////////////////////////////////
char dis[100];

const char* ALU[] = {
		EP(ALU_OP_T),
		EP(ALU_OP_N),
		EP(ALU_OP_R),
		EP(ALU_OP_GET),
		EP(ALU_OP_PUT),
		EP(ALU_OP_DPLUS),
		EP(ALU_OP_DMUL),
		EP(ALU_OP_AND),
		EP(ALU_OP_OR),
		EP(ALU_OP_XOR),
		EP(ALU_OP_NEG),
		EP(ALU_OP_DEC),
		EP(ALU_OP_EQ0),
		EP(ALU_OP_EQ),
		EP(ALU_OP_UCMP),
		EP(ALU_OP_CMP),
		EP(ALU_OP_RSHIFT),
		EP(ALU_OP_LSHIFT),
		EP(ALU_OP_SP),
		EP(ALU_OP_RS),
		EP(ALU_OP_SETSP),
		EP(ALU_OP_SETRP),
		EP(ALU_OP_ST),
		EP(ALU_OP_SETST),
		EP(ALU_OP_TX),
		EP(ALU_OP_RX),
		EP(ALU_OP_UMOD),
		EP(ALU_OP_MOD),
		EP(ALU_OP_REG),
		EP(ALU_OP_SETREG),
		EP(ALU_OP_NOOP__),
		EP(ALU_OP_BYE)
};

const char* OPTYPE[] = {
		EP(OP_JMP),
		EP(OP_JZ),
		EP(OP_CALL),
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
	strcpy(dis, "");

	if (word & OP_LIT) {
		sprintf(hex, "%04x", ARG_LIT(word));
		strcat(dis, OPTYPE[OP_LIT]);
		strcat(dis, " ");
		strcat(dis, hex);
	} else {
		switch (OP(word)) {
		case OP_JZ:
			sprintf(hex, "%04x",  ARG_OP(word));
			strcat(dis, OPTYPE[OP_JZ]);
			strcat(dis, " ");
			strcat(dis, hex);
			break;
		case OP_JMP:
			sprintf(hex, "%04x",  ARG_OP(word));
			strcat(dis, OPTYPE[OP_JMP]);
			strcat(dis, " ");
			strcat(dis, hex);
			break;
		case OP_CALL:
			sprintf(hex, "%04x",  ARG_OP(word));
			strcat(dis, OPTYPE[OP_CALL]);
			strcat(dis, " ");
			strcat(dis, hex);
			break;
		case OP_ALU:
			strcpy(dis, ALU[ALU_OP(word)]);
			strcat(dis, " (");
			if (word & ALU_F_R2P) {
				strcat(dis, FLAGS[ALU_F_R2P]);
				strcat(dis, "/");
			}
			if (word & ALU_F_T2N) {
				strcat(dis, FLAGS[ALU_F_T2N]);
				strcat(dis, "/");
			}
			if (word & ALU_F_T2R) {
				strcat(dis, FLAGS[ALU_F_T2R]);
				strcat(dis, "/");
			}
			if (word & ALU_F_N2T) {
				strcat(dis, FLAGS[ALU_F_N2T]);
				strcat(dis, "/");
			}

			strcat(dis, "dp=");
			sprintf(hex, "%d/", _delta[ALU_DS(word)]);
			strcat(dis, hex);
			strcat(dis, "dp=");
			sprintf(hex, "%d)", _delta[ALU_RS(word)]);
			strcat(dis, hex);
		}
	}

	return dis;
}
