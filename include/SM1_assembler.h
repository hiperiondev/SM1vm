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
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <utils/jwHash.h>

#define UNDER_OVER
//#define CARRY
#define EXTRAREGS
//#define EXTBITS

/////////////////////////////////////////////////////////////////////////////////////

const uint8_t  VMFLAGS_POS[] = { ALU_F_T2N, ALU_F_T2R, ALU_F_N2T, ALU_F_R2P };
const uint8_t VMFLAGS_CODE[] = { 0x80, 0x40, 0x20, 0x10 };
 char stringResult[512];

 int macroIndex = 0;
char macroName[40] = "";
 int addr = -1;
 int isStr = 0;
 int asmPrecedent = 0;
bool isAlu = false;
bool isCall = false;

jwHashTable*   equ = NULL;
jwHashTable*  word = NULL;
jwHashTable* label = NULL;
jwHashTable* macro = NULL;

/////////////////////////////////////////////////////////////////////////////////////

     int sm1_assembleFile(char* fileIn, char* fileOut);
uint16_t sm1_assembleLine(char* line, bool pass);

/////////////////////////////////////////////////////////////////////////////////////

int opCmp(char *op, char *value){
    return strcmp(strlwr(op), value);
}

int doHere(char *item1, char *item2) {
    int offsetHere = 0;
    char *ptr;
    if (strcmp(item1, "+") == 0) {
        int res = (int) strtol(item2, &ptr, 10);
        if (item2 == ptr) {
            printf("ASSEMBLER ERROR: $HERE$ offset not number\n");
            exit(1);
        }
        offsetHere += res;
    }
    if (strcmp(item1, "-") == 0) {
        int res = (int) strtol(item2, &ptr, 10);
        if (item2 == ptr) {
            printf("ASSEMBLER ERROR: $HERE$ offset not number\n");
            exit(1);
        }
        offsetHere -= res;
    }
    return (addr + offsetHere);


}

int directives(char* line, char* fileOut, bool pass) {
    char lineSplited[40][80], str[40] = "";
     int words = getWords(line, lineSplited);
    char * hresult;

    // search macro and insert
    if (get_str_by_str(macro, lineSplited[0], &hresult) != HASHNOTFOUND) {
        char macroArgs[40][80];
        char macroLine[40][80];
         int macroArgsCnt = words - 1;
         int macroArgsCntTmp = macroArgsCnt;
        while (macroArgsCntTmp != 0) {
            strcpy(macroArgs[macroArgsCntTmp], lineSplited[macroArgsCntTmp]);
            --macroArgsCntTmp;
        }
        macroIndex = 1;
        strcpy(macroName, lineSplited[0]);
        printf("[ %s\n", macroName);
        while (1) {
            sprintf(str, "%d", macroIndex++);
            strcat(str, "#_");
            strcat(str, macroName);
            if (get_str_by_str(macro, str, &hresult) == HASHNOTFOUND)
                break;
            words = getWords(hresult, macroLine);
            strcpy(hresult, "");
            for (int n = 0; n <= words - 1; n++) {
                if (macroLine[n][0] == '@') {
                    if (!isdigit(macroLine[n][1]) || ((macroLine[n][1] - '0') >= macroArgsCnt))
                        break;
                    strcpy(macroLine[n], macroArgs[macroLine[n][1] - '0']);
                }
                strcat(hresult, macroLine[n]);
                strcat(hresult, " ");
            }

            sm1_assembleLine(hresult, pass);
        }
        printf("]\n");
        macroIndex = 0;
        return 0;
    }

    if (opCmp(lineSplited[0], ".comment") == 0) {
        if (pass)
            printf("%s\n", line);
        return 0;
    }
    if (opCmp(lineSplited[0], ".equ") == 0) {
        if (opCmp(lineSplited[2], "$here$") == 0) {
            sprintf(lineSplited[2], "%04x", doHere(lineSplited[3],lineSplited[4]));
        }
        if (pass) {
            add_str_by_str(equ, lineSplited[1], lineSplited[2]);
            get_str_by_str(equ, lineSplited[1], &hresult);
            printf(".equ %s %s\n", lineSplited[1], hresult);
        }
        return 0;
    }
    if (opCmp(lineSplited[0], ".macro") == 0) {
        if (macroIndex) {
            printf("ASSEMBLER ERROR: macro inside macro\n");
            exit(1);
        }
        macroIndex = 1;
        if (pass) {
            strcpy(macroName, lineSplited[1]);
            add_str_by_str(macro, macroName, macroName);
            get_str_by_str(macro, macroName, &hresult);
            printf(".macro %s \n", hresult);
        }
        return 0;
    }
    if (opCmp(lineSplited[0], ".endm") == 0) {
        if(pass) printf(".endm\n");
        macroIndex = 0;
        return 0;
    }
    if (opCmp(lineSplited[0], ".include") == 0) {
        printf(".include %s\n", lineSplited[1]);
        sm1_assembleFile(lineSplited[1], fileOut);
        printf(".endinclude\n");
        return 0;
    }
    if (opCmp(lineSplited[0], ".word") == 0) {
        int ptr = 2;
        while (ptr <= words) {
            strcat(str, lineSplited[ptr]);
            strcat(str, " ");
            ptr++;
        }
        get_str_by_str(word, lineSplited[1], &hresult);
        if (pass) {
            add_str_by_str(word, lineSplited[1], str);
            get_str_by_str(word, lineSplited[1], &hresult);
            printf(".word %s (%s)\n", lineSplited[1], hresult);
        }
        return 0;
    }
    if (opCmp(lineSplited[0], ".label") == 0) {
        sprintf(str, "%04x", addr + 1);
        if (pass)
            add_str_by_str(label, lineSplited[1], str);
        get_str_by_str(label, lineSplited[1], &hresult);
        printf(".label %s (%s)\n", lineSplited[1], hresult);
        return 0;
    }
    if (opCmp(lineSplited[0], ".org") == 0) {
        char *ptr;
        int value;
        if (opCmp(lineSplited[1], "$here$") == 0) {
            value = doHere(lineSplited[2], lineSplited[3]);
            printf("     ^_ (%04x)\n", value);
        } else {
            value = (int) strtol(lineSplited[1], &ptr, 10);
            if (lineSplited[1] == ptr) {
                perror("Error: value not decimal integer");
                return RC_ERROR;
            }
        }
        addr = value;
        return 0;
    }

    if (macroIndex) {
        sprintf(str, "%d", macroIndex++);
        strcat(str, "#_");
        strcat(str, macroName);
        if (pass)
            add_str_by_str(macro, str, line);
    }

    return 1;
}

//////////////////////////////////////////////////////////////

uint16_t sm1_assembleLine(char* line, bool pass) {
      int words, value, w;
     //bool lit = false;
     char *hresult = NULL;
     char lineSplited[40][80], str[40];
      int len = 0;
     char *ptr;

    isStr = 0;
    words = getWords(line, lineSplited);
    value = (int) strtol(lineSplited[1], &ptr, 16);
    //if (lineSplited[1] != ptr) {
        //lit = true;
    //}

    addr++;
    if (!pass) {
        printf("    %04x ", addr);
        printf("%s\n", line);
    }

    get_str_by_str(word, lineSplited[0], &hresult);
    if (hresult != NULL) {
        words = getWords(hresult, lineSplited);
    } else {
        get_str_by_str(equ, lineSplited[1], &hresult);
        if (hresult == NULL)
            get_str_by_str(label, lineSplited[1], &hresult);

        if (hresult != NULL) {
            value = (int) strtol(hresult, NULL, 16);
            printf("               ^_ (%04x)\n", value);
        } //else {
          //  if (!lit && !pass) {
          //      printf("             ^_ ERROR: unknown value\n");
          //  }
        //}
    }

    if (opCmp(lineSplited[0], "lit") == 0) {
        if (opCmp(lineSplited[1], "$here$") == 0) {
            value = doHere(lineSplited[2], lineSplited[3]);
            printf("             ^_ (%04x)\n", value);
        }
        if (value < 32768)
            return (0x8000 | value);
        printf("ASSEMBLER ERROR: lit too long\n");
        exit(1);
    }

    if (opCmp(lineSplited[0], "jmp") == 0) {
        if (opCmp(lineSplited[1], "$here$") == 0) {
            value = doHere(lineSplited[2], lineSplited[3]);
            printf("             ^_ (%04x)\n", value);
        }
        if (value < 8192)
            return (0x0000 | value);
        printf("ASSEMBLER ERROR: jmp too long\n");
        exit(1);
    }
    if (opCmp(lineSplited[0], "jmz") == 0) {
        if (opCmp(lineSplited[1], "$here$") == 0) {
            value = doHere(lineSplited[2], lineSplited[3]);
            printf("             ^_ (%04x)\n", value);
        }
        if (value < 8192)
            return (0x2000 | value);
        printf("ASSEMBLER ERROR: jmz too long\n");
        exit(1);
    }
    if (opCmp(lineSplited[0], "cll") == 0) {
        if (opCmp(lineSplited[1], "$here$") == 0) {
            value = doHere(lineSplited[2], lineSplited[3]);
            printf("             ^_ (%04x)\n", value);
        }
        if (value < 8192)
            return (0x4000 | value);
        printf("ASSEMBLER ERROR: cll too long\n");
        exit(1);
    }

    if (opCmp(lineSplited[0], ".data") == 0) {
        if (opCmp(lineSplited[1], "$here$") == 0) {
            value = doHere(lineSplited[2], lineSplited[3]);
            printf("       ^_ (%04x)\n", value);
        }
        if (value < 65536) {
            return value;
        }
        printf("ASSEMBLER ERROR: .data too long\n");
        exit(1);
    }
    if (opCmp(lineSplited[0], ".string") == 0) {
        strcpy(str, "");
        int cnt = 1;
        do {
            strcat(str, lineSplited[cnt]);
            strcat(str, " ");
            cnt++;
        } while (cnt < words - 1);
        if (words > 1)
            strcat(str, lineSplited[words - 1]);
        len = strlen(str);
        if ((len % 2) == 1) {
            strcat(str, "\0");
            len++;
        }
        strcpy(stringResult, "");
        char tmpStr[20];
        cnt = 0;
        while (cnt < len) {
            sprintf(tmpStr, "%02x%02x\n", str[cnt], str[cnt + 1]);
            cnt += 2;
            isStr++;
            strcat(stringResult, tmpStr);
        }
        return 0;
    }

    value = 0xffff;
    for (int w = 0; w < 32; w++) {
        strcpy(str, ALU[w]);
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
    if (!pass)
        printf("         ^_ %s\n", hresult);
    value |= 0x6000;

    int pos = 1, fl = 0;
    while (pos < words) {
        for (w = 0; w < 4; w++) {
            strcpy(str, VMFLAGS[VMFLAGS_POS[w]]);
            removePrefix(str);
            if (opCmp(str, lineSplited[pos]) == 0) {
                value |= VMFLAGS_CODE[w];
                fl = 1;
                break;
            }
        }
        pos++;
        for (w = 0; w < 6; w++) {
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
    FILE *fIn;
    FILE *fOut;
    char buf[80];
    int asmResult;

    if (equ == NULL) {
          equ = create_hash(100);
         word = create_hash(100);
        label = create_hash(100);
        macro = create_hash(1000);
        remove(fileOut);
    }

    if ((fIn = fopen(fileIn, "r")) == NULL) {
        perror("Error: can't open source-file");
        return 1;
    }

    if ((fOut = fopen(fileOut, "a")) == NULL) {
        perror("Error: can't open destination-file");
        return 1;
    }

    printf ("\n\n -- First pass --\n\n");
    while (fgets(buf, sizeof(buf), fIn) != NULL) {
        buf[strlen(buf) - 1] = '\0';
        if (strcmp(buf, "") != 0) {
            if (directives(buf, fileOut, true) && !macroIndex) {
                asmResult = sm1_assembleLine(buf, true);
                if (isStr > 0) {
                    addr += isStr;
                }else {
                    /* (r2p r-1)|CALL/JMP optimization */
                    if ((asmResult & OP_ALU) == OP_ALU) {
                        if ((asmResult == 0x6018) && (isAlu)
                                && !(asmPrecedent & 0x1C)) {
                            fprintf(fOut, "%04x\n", asmPrecedent | ALU_F_R2P);
                            isAlu = false;
                            printf("            ^_  compress R2P\n");
                            addr--;
                            continue;
                        }
                        if ((asmResult == 0x6018) && isCall) {
                            fprintf(fOut, "%04x\n", asmPrecedent);
                            isCall = false;
                            printf("            ^_  compress CALL/JMP\n");
                            addr--;
                            continue;
                        }
                        if (asmResult == 0x6018) {
                            if (isAlu)
                                fprintf(fOut, "%04x\n", asmPrecedent);
                            fprintf(fOut, "%04x\n", asmResult);
                            isAlu = false;
                            continue;
                        }
                        asmPrecedent = asmResult;
                        isAlu = true;
                        continue;
                    }

                    if ((asmResult & OP_CLL) == OP_CLL) {
                        asmPrecedent = ARG_OP(asmResult);
                        isCall = true;
                        continue;
                    }
                    /***********************/

                     isAlu = false;
                    isCall = false;
                    fprintf(fOut, "%04x\n", asmResult);
                }
            }
        }
    }

    addr = -1;
    rewind(fIn);
    printf ("\n\n -- Second pass --\n\n");
    while (fgets(buf, sizeof(buf), fIn) != NULL) {
        buf[strlen(buf) - 1] = '\0';
        if (strcmp(buf, "") != 0) {
            if (directives(buf, fileOut, false) && !macroIndex) {
                asmResult = sm1_assembleLine(buf, false);
                if (isStr > 0) {
                    addr += isStr;
                    fprintf(fOut, stringResult);
                } else {
                    /* (r2p r-1)|CALL/JMP optimization */
                    if ((asmResult & OP_ALU) == OP_ALU) {
                        if ((asmResult == 0x6018) && (isAlu)
                                && !(asmPrecedent & 0x1C)) {
                            fprintf(fOut, "%04x\n", asmPrecedent | ALU_F_R2P);
                            isAlu = false;
                            printf("            ^_  compress R2P\n");
                            addr--;
                            continue;
                        }
                        if ((asmResult == 0x6018) && isCall) {
                            fprintf(fOut, "%04x\n", asmPrecedent);
                            isCall = false;
                            printf("            ^_  compress CALL/JMP\n");
                            addr--;
                            continue;
                        }
                        if (asmResult == 0x6018) {
                            if (isAlu)
                                fprintf(fOut, "%04x\n", asmPrecedent);
                            fprintf(fOut, "%04x\n", asmResult);
                            isAlu = false;
                            continue;
                        }
                        asmPrecedent = asmResult;
                        isAlu = true;
                        continue;
                    }

                    if ((asmResult & OP_CLL) == OP_CLL) {
                        asmPrecedent = ARG_OP(asmResult);
                        isCall = true;
                        continue;
                    }
                    /***********************/

                    isAlu = false;
                    isCall = false;
                    fprintf(fOut, "%04x\n", asmResult);
                }
            }
        }
    }
    fclose(fIn);

    return 0;
}
