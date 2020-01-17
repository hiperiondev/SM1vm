/*
 * SM1_disassembler.h
 *
 *  License: GNU GPL V3
 *
 *  Created on: 15 jun. 2018
 *  Author: Emiliano Augusto Gonzalez (egonzalez.hiperion@gmail.com)
 */

char dis[100];

char* sm1_disassembly(uint16_t word) {
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
                strcat(dis, VMFLAGS[ALU_F_R2P]);
            }
            if (word & ALU_F_T2N) {
                if (f==1) strcat(dis, "|");
                f = 1;
                strcat(dis, VMFLAGS[ALU_F_T2N]);
            }
            if (word & ALU_F_T2R) {
                if (f==1) strcat(dis, "|");
                f = 1;
                strcat(dis, VMFLAGS[ALU_F_T2R]);
            }
            if (word & ALU_F_N2T) {
                if (f==1) strcat(dis, "|");
                strcat(dis, VMFLAGS[ALU_F_N2T]);
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

    removePrefix(dis);
    return strlwr(dis);
}
