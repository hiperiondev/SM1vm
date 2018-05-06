/*
 * main.c
 *
 *  Created on: 6 abr. 2018
 *      Author: emiliano gonzalez (egonzalez.hiperion@gmail.com)
 */

#include <stdlib.h>
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////////////

//#define DEBUG 1
//#define MANUAL_STEP
//#define UNDER_OVER

#define DS_SIZE  65535
#define RS_SIZE  65535
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

uint8_t sm1_mem_load(uint16_t addr, vm_t* vm) {
        FILE *RAM;
        RAM = fopen("forth/eforth.rom", "r");
        if (RAM == NULL)
                return RC_ERROR;
        uint16_t data;
        while (fread(&data, sizeof(uint16_t), 1, RAM) == 1) {
                vm->RAM[addr++] = data;
        }
        fclose(RAM);

        return RC_OK;
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
        uint8_t result;

#ifdef DEBUG
        int step_counter = 0;

        DBG_PRINT("RESET...\n");
#endif
        sm1_reset(vm);
#ifdef DEBUG
        DBG_PRINT("LOAD...\n");
#endif
        if (sm1_mem_load(0x0000, vm) == RC_ERROR) {
#ifdef DEBUG
        	DBG_PRINT("...Can't load file!\n");
#endif
            exit(1);
        }
#ifdef DEBUG
        DBG_PRINT("START...\n");
#endif
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
