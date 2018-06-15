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

/////////////////////////////////////////////////////////////////////////////////////

//#define DEBUG 1
//#define MANUAL_STEP
#define UNDER_OVER
//#define CARRY
#define EXTRAREGS
//#define EXTBITS
#define REG_SIZE 2
#define DS_SIZE  128
#define RS_SIZE  128
#define RAM_SIZE 65535

/////////////////////////////////////////////////////////////////////////////////////

#include "SM1.h"

uint8_t sm1_mem_put(uint16_t addr, uint16_t value, vm_t* vm) {
        vm->RAM[addr] = value;
        return RC_OK;
}

uint16_t sm1_mem_get(uint16_t addr, vm_t* vm) {
        return vm->RAM[addr];
}


/////////////////////////////////////////////////////////////////////////////////////

int main() {
        vm_t *vm     = (vm_t *)     malloc(sizeof(vm_t));
        vm->RAM      = (uint16_t *) malloc(sizeof(uint8_t)  * RAM_SIZE);
        vm->rs       = (uint16_t *) malloc(sizeof(uint16_t) * RS_SIZE);
        vm->ds       = (uint16_t *) malloc(sizeof(uint16_t) * DS_SIZE);
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
        DBG_PRINT("LOAD...\n");
#endif
	FILE *RAM;
	int addr = 0x0000;
	RAM = fopen("/tmp/eforth.rom", "r");
#ifdef DEBUG
	if (RAM == NULL)
        	DBG_PRINT("...Can't load file!\n");
#endif
	uint16_t data;
	while (fread(&data, sizeof(uint16_t), 1, RAM) == 1) {
		vm->RAM[addr++] = data;
	}
	fclose(RAM);
#ifdef DEBUG
        DBG_PRINT("START...\n");
#endif
       vm->pc = vm->RAM[0];
        while (1) {
                uint16_t word = sm1_mem_get(vm->pc, vm);
                result = sm1_step(word, vm);
#ifdef DEBUG
                DBG_PRINT("step:%d\n", step_counter++);
#endif
                if (vm->status & ST_SNDTN) {
#ifdef DEBUG
                	DBG_PRINT("%c", (char) vm->t_ext);
#endif
                	vm->status &= ~ST_SNDTN;
                }

                if (result != RC_OK) {
#ifdef DEBUG
                	DBG_PRINT("EXCEPTION: %02x\n", result);
#endif
                    exit(1);
                }
#ifdef MANUAL_STEP
                getchar();
#endif
        }
}
