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
/////////////////////////////////////////////////////////////////////////////////////

//#define DEBUG 1
#define KEYBOARD_ENTRY
#define UNDER_OVER
//#define CARRY
#define EXTRAREGS
//#define EXTBITS
#define REG_SIZE 2
#define DS_SIZE  128
#define RS_SIZE  128
#define RAM_SIZE 65535

/////////////////////////////////////////////////////////////////////////////////////

#include "SM1_disassembler.h"
#include "SM1.h"

uint8_t sm1_mem_put(uint16_t addr, uint16_t value, vm_t* vm) {
        vm->RAM[addr] = value;
        return RC_OK;
}

uint16_t sm1_mem_get(uint16_t addr, vm_t* vm) {
        return vm->RAM[addr];
}

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
	vm_t *vm =     (vm_t *) malloc(sizeof(vm_t));
	vm->RAM  = (uint16_t *) malloc(sizeof(uint8_t) * RAM_SIZE);
	vm->rs   = (uint16_t *) malloc(sizeof(uint16_t) * RS_SIZE);
	vm->ds   = (uint16_t *) malloc(sizeof(uint16_t) * DS_SIZE);
#ifdef UNDER_OVER
	vm->ds_size  = DS_SIZE;
	vm->rs_size  = RS_SIZE;
	vm->RAM_size = RAM_SIZE;
#endif
#ifdef EXTRAREGS
	vm->reg      = (uint16_t *) malloc(sizeof(uint8_t) * REG_SIZE);
	vm->reg_size = REG_SIZE;
#endif
    uint8_t result;
#ifdef DEBUG
    int step_counter = 0;
    DBG_PRINT("RESET...\n");
#endif
	vm->pc = 0;
	vm->dp = 0;
	vm->rp = 0;

#ifdef DEBUG
        DBG_PRINT("START...\n");
#endif
	if (argc == 1) {
		printf("\nUse: SM1 [-dis] file\n");
		exit(1);
	}

#ifdef DEBUG
    DBG_PRINT("LOAD...\n");
#endif
	FILE *RAM;
	int addr = 0x0000;
	if (argc == 2) {
		RAM = fopen(argv[1], "r");
	} else {
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
    if (!strcmp(argv[1],"-dis")) {
    		int add;
    		for (add=0;add<RAM_SIZE;add++) {
    			uint16_t word = sm1_mem_get(add, vm);
    			printf("%04x %s\n", add, sm1_disasembly(word));
    		}
    } else {
		printf("--START\n\n");
		while (1) {
			uint16_t word = sm1_mem_get(vm->pc, vm);
			result = sm1_step(word, vm);
#ifdef DEBUG
			DBG_PRINT("step:%d\n", step_counter++);
#endif
			if ((vm->status & ST_SNDTN) && (vm->n_ext == 0)) {
				printf ("%c", (char) vm->t_ext);
				vm->status &= ~ST_SNDTN;
			}

			if (result != RC_OK) {
				printf("\nEXCEPTION: %02x\n", result);
				exit(1);
			}
#ifdef KEYBOARD_ENTRY
			if (kbhit()) {
				vm->t_ext = getc(stdin);
				vm->status |= ST_RCVTN;
			}
#endif
		}
	}
}
