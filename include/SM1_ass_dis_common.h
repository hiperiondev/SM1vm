/*
 * SM1_ass_dis_common.h
 *
 *  Created on: 25 jun. 2018
 *      Author: egonzalez
 */

#include <ctype.h>
#include <string.h>
#define EP(x) [x] = #x  /* enum print */

char* strlwr(char *str) {
	unsigned char *p = (unsigned char *) str;
	while (*p) {
		*p = tolower((unsigned char) *p);
		p++;
	}
	return str;
}

char *removeSubStr(char *str, const char *substr) {
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

void removePrefix(char *str) {
	strcpy(str, removeSubStr(str, "ALU_"));
	strcpy(str, removeSubStr(str, "OP_"));
	strcpy(str, removeSubStr(str, "F_"));
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
		EP(OP_LIT),
		EP(OP_ALU),
		EP(OP_JMP),
		EP(OP_JMZ),
		EP(OP_CLL)
};

const char* VMFLAGS[] = {
		EP(ALU_F_T2N),
		EP(ALU_F_T2R),
		EP(ALU_F_N2T),
		EP(ALU_F_R2P)
};

const char* DELTA[] = {
       "d+1",
       "d-1",
       "d-2",
       "r+1",
       "r-1",
       "r-2"
};

const uint8_t DELTA_CODE[] = { 0x1, 0x2, 0x3, 0x4, 0x8, 0xC };
