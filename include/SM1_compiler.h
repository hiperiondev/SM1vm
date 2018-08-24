/*
 * SM1_compiler.h
 *
 *  License: GNU GPL V3
 *
 *  Created on: 26 Jul. 2018
 *  Author: Emiliano Augusto Gonzalez (egonzalez.hiperion@gmail.com)
 */

#ifndef SM1_COMPILER_H_
#define SM1_COMPILER_H_

#include "SM1.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <utils/utarray.h>
#include <utils/switchs.h>

#define UNDER_OVER
//#define CARRY
#define EXTRAREGS
//#define EXTBITS

/////////////////////////////////////////////////////////////////////////////////////

char bWords[100][20];
 int bW = 0;
 int addrC = 0;
 int ifCnt = 1;
 int ifStk[20];
 int ifStkP = -1;
 int beginCnt = 1;
 int beginStk[20];
 int beginStkP = -1;
 int doCnt = 1;
 int doStk[20];
 int doStkP = -1;
bool comment = false;
 int tupleCnt = 0;
char tuple[3][20];

struct Header {
     int type; /* 0:create, 1: colon, 2: variable, 3:constant */
    char name[40];
     int cfa;
    bool immediate;
    bool compileOnly;
    bool included;
    UT_array *headerReg;
    struct Header *next;
};

struct Header *header = NULL;
struct Header *lastHeader = NULL;

/////////////////////////////////////////////////////////////////////////////////////

void doHeader(struct Header** head_ref, char *name) {
    struct Header* new_header = (struct Header*) malloc(sizeof(struct Header));
    utarray_new(new_header->headerReg, &ut_int_icd);
    strcpy(new_header->name, name);
    new_header->type = 0;
    new_header->immediate = false;
    new_header->compileOnly = false;
    new_header->cfa = 0;
    new_header->next = (*head_ref);
    lastHeader = new_header;
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
    char typeHeader[][10] = { "create", "colon", "variable", "constant" };
    printf("\nHeaders:\n");
    while (node != NULL) {
        printf("[%s]\n", node->name);
        printf("     type: %s\n", typeHeader[node->type]);
        printf("      cfa: %04x\n", node->cfa);
        printf("    immed: %s\n", node->immediate ? "true" : "false");
        printf(" compOnly: %s\n", node->compileOnly ? "true" : "false");
        printf("      Reg: %d\n\n", utarray_len(node->headerReg));
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

int readTuple(FILE* fIn) {
    static int eofCnt;
    while (tupleCnt < 3) {
        strcpy(tuple[0], tuple[1]);
        strcpy(tuple[1], tuple[2]);
        if (fscanf(fIn, "%s", tuple[2]) == EOF) {
            strcpy(tuple[2], "\000");
            eofCnt++;
        }
        tupleCnt++;
        if (eofCnt == 2)
            return 0;
    }
    return 1;
}

/////////////////////////////////////////////////////////////////////////////////////

void doCreate(char *name) {
    doHeader(&header, name);
}

void doCol(char *name) {
    doHeader(&header, name);
    lastHeader->type = 1;
    lastHeader->cfa = addrC;
}

void doVar(char *name) {
    int value = 0;
    doHeader(&header, name);
    lastHeader->type = 2;
    utarray_push_back(lastHeader->headerReg, &value);
}

void doConst(char *name, int value) {
    doHeader(&header, name);
    lastHeader->type = 3;
    utarray_push_back(lastHeader->headerReg, &value);
}

char* doTick(char *word) {
    static char resultStr[50];
    struct Header *search = searchHeader(header, word);
    if (search != NULL) {
        sprintf(resultStr, "    lit %s", word);
        return resultStr;
    }
    return "!!ERROR!!";
}
void doAllot(int qty) {
    int val = 0;
    for (int n = 0; n < qty; n++)
        utarray_push_back(lastHeader->headerReg, &val);
}

void doComma(int res) {
    utarray_push_back(lastHeader->headerReg, &res);
}

char* doLit(int number) {
    if ((number > 0xffff) || (number < 0))
        return "!!ERROR!!";
    char literal[9];
    static char resultStr[20];
    if (number < 0x8000) {
        sprintf(literal, "%04x", (short) number);
        sprintf(resultStr, "    lit ");
        strcat(resultStr, literal);
    } else {
        sprintf(literal, "%04x", ~(short) number);
        sprintf(resultStr, "    lit ");
        strcat(resultStr, literal);
        strcat(resultStr, "\n    neg");
    }
    return resultStr;
}


/////////////////////////////////////////////////////////////////////////////////////

char* compileTuple() {
    static char compiledTuple[50];
    strcpy(compiledTuple, "");
    static bool comment = false;

    switchs(tuple[0])
            {
            cases("(")
                comment = true;
                --tupleCnt;
                break;
            cases(")")
                comment = false;
                --tupleCnt;
                break;
            cases(":")
                doCol(tuple[1]);
                sprintf(compiledTuple, ".label doCol_%s", tuple[1]);
                lastHeader->included = true;
                tupleCnt -= 2;
                break;
            cases("*:")
                doCol(tuple[1]);
                sprintf(compiledTuple, ".label doCol_%s", tuple[1]);
                lastHeader->included = false;
                tupleCnt -= 2;
                break;
            cases(";")
                strcpy(compiledTuple, "    exit");
                --tupleCnt;
                if (!strcmp(tuple[1], "immediate")) {
                    lastHeader->immediate = true;
                    --tupleCnt;
                }
                break;
            cases("variable")
                sprintf(compiledTuple, ".comment doVar %s", tuple[1]);
                doVar(tuple[1]);
                tupleCnt -= 2;
                break;
            cases("constant")
            cases("*constant")
                printf("ERROR: constant not initialized\n");
                strcpy(compiledTuple, "!!ERROR!!");
                --tupleCnt;
                break;
            cases("'")
                strcpy(compiledTuple, doTick(tuple[1]));
                if (!strcmp(compiledTuple, "!!ERROR!!")) {
                    printf("ERROR: doTick - word not found\n");
                }
                tupleCnt -= 2;
                break;
            cases(",")
            cases("c,")
                printf("ERROR: comma not initialized\n");
                strcpy(compiledTuple, "!!ERROR!!");
                --tupleCnt;
                break;
            cases("allot")
                printf("ERROR: allot not initialized\n");
                strcpy(compiledTuple, "!!ERROR!!");
                --tupleCnt;
                break;
            cases("if")
                sprintf(compiledTuple, "    jmz if_%04x", ifCnt);
                ifStkP++;
                ifStk[ifStkP] = ifCnt++;
                --tupleCnt;
                break;
            cases("then")
                if (ifStkP == -1) {
                   printf("ERROR: then without if\n");
                   strcpy(compiledTuple, "!!ERROR!!");
                   --tupleCnt;
                   break;
                }
                char elseC[6];
                strcpy(elseC, "");
                if (ifStk[ifStkP] == 0) {
                   strcpy(elseC, "_else");
                   ifStkP--;
                }
                sprintf(compiledTuple, ".label if_%04x%s", ifStk[ifStkP--], elseC);
                --tupleCnt;
                break;
            cases("else")
                if (ifStkP == -1) {
                    printf("ERROR: else without if\n");
                    strcpy(compiledTuple, "!!ERROR!!");
                    --tupleCnt;
                    break;
                }
                sprintf(compiledTuple, "    jmp if_%04x_else\n.label if_%04x",
                        ifStk[ifStkP], ifStk[ifStkP]);
                ifStk[++ifStkP] = 0;
                --tupleCnt;
                break;
            cases("begin")
                sprintf(compiledTuple, ".label begin_%04x", beginCnt);
                beginStkP++;
                beginStk[beginStkP] = beginCnt++;
                --tupleCnt;
                break;
            cases("again")
                if (beginStkP == -1) {
                    printf("ERROR: again without begin\n");
                    strcpy(compiledTuple, "!!ERROR!!");
                    --tupleCnt;
                    break;
                }
                sprintf(compiledTuple, "    jmp begin_%04x",
                        beginStk[beginStkP]);
                --beginStkP;
                --tupleCnt;
                break;
            cases("until")
                if (beginStkP == -1) {
                    printf("ERROR: again without begin\n");
                    strcpy(compiledTuple, "!!ERROR!!");
                    --tupleCnt;
                    break;
                }
                sprintf(compiledTuple,
                        "    jmz begin_%04x\n.label begin_%04x_end",
                        beginStk[beginStkP], beginStk[beginStkP]);
                --beginStkP;
                --tupleCnt;
                break;
            cases("while")
                if (beginStkP == -1) {
                    printf("ERROR: while without begin\n");
                    strcpy(compiledTuple, "!!ERROR!!");
                    --tupleCnt;
                    break;
                }
                sprintf(compiledTuple, "    jmz begin_%04x_end",
                        beginStk[beginStkP]);
                --tupleCnt;
                break;
            cases("repeat")
                if (beginStkP == -1) {
                    printf("ERROR: repeat without begin\n");
                    strcpy(compiledTuple, "!!ERROR!!");
                    --tupleCnt;
                    break;
                }
                sprintf(compiledTuple,
                        "    jmp begin_%04x\n.label begin_%04x_end",
                        beginStk[beginStkP], beginStk[beginStkP]);
                --beginStkP;
                --tupleCnt;
                break;
            cases("leave")
                if (beginStkP == -1) {
                    printf("ERROR: leave without begin\n");
                    strcpy(compiledTuple, "!!ERROR!!");
                    --tupleCnt;
                    break;
                }
                sprintf(compiledTuple, "    jmp begin_%04x_end",
                        beginStk[beginStkP], beginStk[beginStkP]);
                --beginStkP;
                --tupleCnt;
                break;
            cases("do") //TODO: optimization: insert values in rstk
                sprintf(compiledTuple, "   >r\n   >r\n.label do_%04x", doCnt);
                doStkP++;
                doStk[doStkP] = doCnt++;
                --tupleCnt;
                break;
            cases("loop")
                if (doStkP == -1) {
                    printf("ERROR: loop without do\n");
                    strcpy(compiledTuple, "!!ERROR!!");
                    --tupleCnt;
                    break;
                }
                sprintf(compiledTuple, "    cri\n    jmz do_%04x",
                        beginStk[beginStkP]);
                --beginStkP;
                --tupleCnt;
                break;
            defaults
            }switchs_end;

    if (comment || (tupleCnt < 3))
        return compiledTuple;

    // base words
    if (searchBwords(tuple[0]) == RC_OK) {
        sprintf(compiledTuple, "    %s",tuple[0]);
        --tupleCnt;
        return compiledTuple;
    }

    // headers
    struct Header *search = searchHeader(header, tuple[0]);
    if (search != NULL) {
        // colon definition
        if (search->type == 1) { // colon definition
            char cfaStr[5];
            sprintf(cfaStr, "%04x", search->cfa);
            strcpy(compiledTuple, "    cll ");
            strcat(compiledTuple, cfaStr);
            --tupleCnt;
            return compiledTuple;
        }
        // variable ( .equ position )
        if (search->type == 2) {
            strcpy(compiledTuple, "    lit ");
            strcat(compiledTuple, tuple[0]);
            --tupleCnt;
            return compiledTuple;
        }
        // constant
        if (search->type == 3) {
            int value = *(int*) utarray_front(search->headerReg);
            strcpy(compiledTuple, doLit(value));
            --tupleCnt;
            return compiledTuple;
        }
    }

    // literal
    char *ptr;
    bool lit = false;
     int res = (int)strtol(tuple[0], &ptr, 10);
    if(tuple[0] != ptr){
        lit = true;
    }
    /*
    else {
        res = strtol(tuple[0], &ptr, 16);
        if(tuple[0] != ptr){
            lit = true;
        }
    }
    */
    if (lit) {
        switchs(tuple[1])
                {
                cases("constant")
                    doConst(tuple[2], res);
                    lastHeader->included = true;
                    tupleCnt -= 3;
                    sprintf(compiledTuple, ".comment doConst %s (%04x)", tuple[1], res);
                    break;
                cases("*constant")
                    doConst(tuple[2], res);
                    lastHeader->included = false;
                    tupleCnt -= 3;
                    sprintf(compiledTuple, ".comment doConst %s (%04x)",
                            tuple[1], res);
                    break;
                cases(",")
                    doComma(res);
                    tupleCnt -= 2;
                    break;
                cases("c,")
                    doComma((uint8_t) res);
                    tupleCnt -= 2;
                    break;
                defaults
                }switchs_end;
        if (tupleCnt < 3)
            return compiledTuple;
        --tupleCnt;
        return doLit(res);
    }

    return "!!ERROR!!";
}

/////////////////////////////////////////////////////////////////////////////////////

int sm1_compileFile(char* fileIn, char* fileOut, char* baseWords, char* ramSizeChar) {
    FILE *fIn;
    FILE *fOut;
    FILE *fWords;
    char buf[80];
    char lineSplited[40][80];
    char compiled[50];
    char *ptr;
     int ramSize = (int) strtol(ramSizeChar, &ptr, 10);

    remove(fileOut);

    if (ramSizeChar == ptr) {
        perror("Error: ramSize not integer");
        return RC_ERROR;
    }

    if ((fIn = fopen(fileIn, "r")) == NULL) {
        perror("Error: can't open source-file");
        return RC_ERROR;
    }

    if ((fOut = fopen(fileOut, "a")) == NULL) {
        perror("Error: can't open destination-file");
        return RC_ERROR;
    }

    if ((fWords = fopen(baseWords, "r")) == NULL) {
        perror("Error: can't open basewords-file");
        return RC_ERROR;
    }
    printf("\n--- start base words %s\n", baseWords);
    fprintf(fOut, ".comment ---------------------------------\n");
    fprintf(fOut, ".comment base words\n\n");
    while (fgets(buf, sizeof(buf), fWords) != NULL) {
        getWords(buf, lineSplited);
        if ((!strcmp(strlwr(lineSplited[0]), ".word"))
                || (!strcmp(strlwr(lineSplited[0]), ".macro"))) {
            strcpy(bWords[bW++], lineSplited[1]);
            printf("%s %s\n", lineSplited[0], lineSplited[1]);
        }
        fprintf(fOut, "%s\n", buf);
    }
    fclose(fWords);
    printf("--- end base words %s\n", baseWords);
    fprintf(fOut, "\n.comment end base words\n");
    fprintf(fOut, ".comment ---------------------------------\n\n");

    //////////////////////////////////////

    printf("\n--- start compiling %s (ramSize = %d)\n", fileIn, ramSize);
    fprintf(fOut, "jmp docol_main\n");

    int res;
    do {
        ++addrC;
        if (addrC > ramSize) {
            printf("ERROR: ramSize exceeded!\n");
            exit(1);
        }
        res = readTuple(fIn);
        strcpy(compiled, compileTuple());
        if (strcmp(compiled, "")) fprintf(fOut, "%s\n", compiled);
        if (!strcmp(strlwr(compiled), "!!error!!")) {
            printf("ERROR: compiler error\n");
            exit(1);
        }
        if (strcmp(compiled, "")) printf ("%s \n",compiled);
    } while (res);

    struct Header *search = searchHeader(header, "main");
    if (search == NULL) {
        printf("Compile Error: not main\n");
        return RC_ERROR;
    }
    printf("--- end compiling %s\n", fileIn);

    ///////////////////////////////////////

    listHeaders(header);
    fclose(fIn);
    // TODO: compile dictionary map
    fclose(fOut);

    return RC_OK;
}

#endif /* SM1_COMPILER_H_ */
