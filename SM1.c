/*
 * main.c
 *
 *  License: GNU GPL V3
 *
 *  Created on: 6 abr. 2018
 *  Author: Emiliano Augusto Gonzalez (egonzalez.hiperion@gmail.com)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define EP(x) [x] = #x  /* enum print */
/////////////////////////////////////////////////////////////////////////////////////

//#define DEBUG 1
#define KEYBOARD_ENTRY
#define UNDER_OVER
//#define CARRY
//#define EXTBITS
//#define AUTOINCR
//#define INDIRECT
#define REG_SIZE 2
#define DS_SIZE  128
#define RS_SIZE  128
#define RAM_SIZE 65535

/////////////////////////////////////////////////////////////////////////////////////

#include "SM1.h"
#include <utils/SM1_common.h>
#include "SM1_assembler.h"
#include "SM1_disassembler.h"
#include "SM1_compiler.h"

uint8_t sm1_mem_put(uint16_t addr, uint16_t value, vm_t* vm) {
    vm->RAM[addr] = value;
    return RC_OK;
}

uint16_t sm1_mem_get(uint16_t addr, vm_t* vm) {
    return vm->RAM[addr];
}

const char* RET[] = {
    EP(RC_OK),
    EP(RC_DS_UNDER_FLOW),
    EP(RC_DS_OVER_FLOW),
    EP(RC_RS_OVER_FLOW),
    EP(RC_RS_UNDER_FLOW),
    EP(RC_PC_OVER_FLOW),
    EP(RC_OP_UNKNOWN),
    EP(RC_ROM_WRITE),
    EP(RC_MEM_OVERFLOW),
    EP(RC_IRQ),
    EP(RC_EXPTN),
    EP(RC_REG_UNKNOWN),
    EP(RC_ERROR),
    EP(RC_BYE)
};
/////////////////////////////////////////////////////////////////////////////////////

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int kbhit(void)
{
  struct termios oldt, newt;
     int ch;
     int oldf;

  tcgetattr(STDIN_FILENO, &oldt);
  newt = oldt;
  newt.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &newt);
  oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
  fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

  ch = getchar();

  tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
  fcntl(STDIN_FILENO, F_SETFL, oldf);

  if(ch != EOF)
  {
    ungetc(ch, stdin);
    return 1;
  }
  return 0;
}

/////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char **argv) {
    uint8_t result;
    vm_t *vm = sm1_init(RAM_SIZE,RS_SIZE, DS_SIZE,  REG_SIZE);

#ifdef DEBUG
    DBG_PRINT("START...\n");
#endif
    if (argc == 1) {
        printf("\nUse: SM1 [-d|-a|-c] file.in [file.out] [basewords] [ramSize]\n");
        exit(1);
    }

    if (!strcmp(argv[1], "-c")) {
        printf("-- COMPILER --\n");
        sm1_compileFile(argv[2], argv[3], argv[4], argv[5]);
    } else
    if (!strcmp(argv[1], "-d")) { // disassembler
        int add;
        for (add = 0; add < RAM_SIZE; add++) {
            uint16_t word = sm1_mem_get(add, vm);
            printf("%04x %s\n", add, sm1_disassembly(word));
        }
    } else
    if (!strcmp(argv[1], "-a")) { // assembler
        sm1_assembleFile(argv[2], argv[3]);
    }
    else {                        // run program
        printf("--RUN\n\n");
#ifdef DEBUG
        DBG_PRINT("LOAD...\n");
#endif
        FILE *RAM;
        int addr = 0x0000;
        if (argc == 2) {
            printf("load(2): %s\n", argv[1]);
            RAM = fopen(argv[1], "r");
        } else {
            printf("load: %s\n", argv[1]);
            RAM = fopen(argv[2], "r");
        }
#ifdef DEBUG
        if (RAM == NULL)
            DBG_PRINT("...Can't load file!\n");
#endif
        uint16_t data;
        while (fread(&data, sizeof(uint16_t), 1, RAM) == 1) {
            vm->RAM[addr++] = data;
        }
        fclose(RAM);

        while (1) {
            uint16_t word = sm1_mem_get(vm->pc, vm);
            result = sm1_step(word, vm);
#ifdef DEBUG
            DBG_PRINT("step:%d\n", step_counter++);
#endif
            if ((vm->status & ST_SNDTN) && (vm->n_ext == 0)) {
                printf("%c", (char) vm->t_ext);
                vm->status &= ~ST_SNDTN;
            }

            if (result != RC_OK) {
                printf("\nEXCEPTION: %s\n", RET[result]);
                exit(1);
            }
#ifdef KEYBOARD_ENTRY
            if (kbhit()) {
                vm->t_ext = getc(stdin);
                vm->n_ext = 0;
                vm->status |= ST_RCVTN;
            }
#endif
        }
    }
}
