/*
 * SM1.h
 *
 *  License: GNU GPL V3
 *
 *  Created on: 6 abr. 2018
 *  Author: Emiliano Augusto Gonzalez (egonzalez.hiperion@gmail.com)
 */

#ifndef __SM1_H__
#define __SM1_H__

#include <inttypes.h>

/////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
#if DEBUG == 1
#define DBG_PRINT(fmt, args...)  \
       fprintf(stderr, "" fmt, ##args)
#endif
#if DEBUG == 2
#define DBG_PRINT(fmt, args...)  \
       fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __FUNCTION__, ##args)
#endif
#else
#define DBG_PRINT(fmt, args...)
#endif

/////////////////////////////////////////////////////////////////////////////////////

#define OP(x)       (x & 0xe000)        /* operand */
#define ARG_OP(x)   (x & 0x1fff)        /* argument of operand */
#define ARG_LIT(x)  (x & 0x7fff)        /* literal */

#define ALU_OP(x)   ((x & 0x1F00) >> 8) /* alu operation */
#define ALU_DS(x)   (x & 0x03)          /* alu data stack */
#define ALU_RS(x)   ((x >> 2) & 0x03)   /* alu return stack */

/////////////////////////////////////////////////////////////////////////////////////

// Operation types
enum {
        OP_JMP  = 0x0000,  /* jump */
        OP_JZ   = 0x2000,  /* jump if zero */
        OP_CALL = 0x4000,  /* call */
        OP_ALU  = 0x6000,  /* alu */
        OP_LIT  = 0x8000   /* literal */
};

// ALU flags
enum {
        ALU_F_T2N = 0x80,   /* move T to N */
        ALU_F_T2R = 0x40,   /* move T to R */
        ALU_F_N2T = 0x20,   /* move N to T */
        ALU_F_R2P = 0x10    /* move R to PC */
};

// ALU operations
enum {
        ALU_OP_T      = 0,  /* T */
        ALU_OP_N      = 1,  /* N */
        ALU_OP_R      = 2,  /* top of return stack */
        ALU_OP_GET    = 3,  /* load from address */
        ALU_OP_PUT    = 4,  /* store to address */
        ALU_OP_DPLUS  = 5,  /* double cell addition */
        ALU_OP_DMUL   = 6,  /* double cell multiply */
        ALU_OP_AND    = 7,  /* bitwise AND */
        ALU_OP_OR     = 8,  /* bitwise OR */
        ALU_OP_XOR    = 9,  /* bitwise XOR */
        ALU_OP_NEG    = 10, /* bitwise inversion */
        ALU_OP_DEC    = 11, /* decrement */
        ALU_OP_EQ0    = 12, /* equal to zero */
        ALU_OP_EQ     = 13, /* equality test */
        ALU_OP_UCMP   = 14, /* unsigned comparison */
        ALU_OP_CMP    = 15, /* signed comparison */
        ALU_OP_RSHIFT = 16, /* logical right shift */
        ALU_OP_LSHIFT = 17, /* logical left shift */
        ALU_OP_SP     = 18, /* depth of stack */
        ALU_OP_RS     = 19, /* R stack depth */
        ALU_OP_SETSP  = 20, /* set stack depth */
        ALU_OP_SETRP  = 21, /* set R stack depth */
        ALU_OP_ST     = 22, /* get status */
        ALU_OP_TX     = 23, /* send T and N */
        ALU_OP_RX     = 24, /* receive T and N */
        ALU_OP_UMOD   = 26, /* u/mod */
        ALU_OP_MOD    = 27, /* /mod */
        ALU_OP_BYE    = 28, /* return */
        ALU_OP_SETST  = 29  /* set status */
};

// Return Condition
enum {
        RC_OK            = 0x00, /* ok */
        RC_DS_UNDER_FLOW = 0x01, /* data stack underflow */
        RC_DS_OVER_FLOW  = 0x02, /* data stack over flow */
        RC_RS_OVER_FLOW  = 0x03, /* return stack over flow */
        RC_RS_UNDER_FLOW = 0x04, /* return stack under flow */
        RC_PC_OVER_FLOW  = 0x05, /* program counter overflow */
        RC_OP_UNKNOWN    = 0x06, /* operator unknown */
        RC_ROM_WRITE     = 0x07, /* rom write */
        RC_MEM_OVERFLOW  = 0x08, /* out of memory access */
        RC_ERROR         = 0xFE, /* generic error */
        RC_BYE           = 0xFF  /* exit */
};

// Status
enum {
        ST_SNDTN = 0x01, /* send */
        ST_RCVTN = 0x02, /* receive */
        ST_RCVQT = 0x04, /* receive quantity*/
        ST_IRQ   = 0x08, /* interrupt */
        ST_IMK   = 0x0f  /* interrupt mask */
};

// Registers
typedef struct {
        int16_t  dp;       /* data stack pointer */
        int16_t  rp;       /* return stack pointer*/
        uint16_t pc;       /* program counter */
        uint16_t t;        /* top of data stack */
        uint16_t t_ext;    /* external top of data stack */
        uint16_t n_ext;    /* external second element of data stack */
        uint8_t  status;   /*  0=SNDTN
                            *  1=RCVTN
                            *  2=NOT USED
                            *  3=IRQ - Interrupt (similar to INTR on Intel 8085)
                            *  4=MASK IRQ
                            *  5=NOT USED
                            *  6=NOT USED
                            *  7=NOT USED
                            */
        uint16_t *RAM;     /* ram */
      //uint16_t *ROM;     /* rom */
        uint16_t *rs;      /* return stack */
        uint16_t *ds;      /* data stack */
#ifdef UNDER_OVER
        uint16_t RAM_size; /* ram size */
      //uint16_t ROM_size; /* rom size */
        uint16_t ds_size;  /* data stack size */
        uint16_t rs_size;  /* return stack size */
#endif

} vm_t;

/////////////////////////////////////////////////////////////////////////////////////

static  uint8_t sm1_mem_put  (uint16_t addr, uint16_t value, vm_t*);
static uint16_t sm1_mem_get  (uint16_t addr, vm_t*);

static inline uint8_t sm1_step(uint16_t word, vm_t* vm) {
#ifdef DEBUG
	    DBG_PRINT("[pc:%04x/dp:%02x/rp:%02x/rs[rp]:%04x/t:%04x/n:%04x/n-1:%04x/n-2:%04x] step(%04x):"
        ,vm->pc, vm->dp, vm->rp, vm->rs[vm->rp], vm->t, vm->ds[vm->dp], vm->ds[vm->dp-1], vm->ds[vm->dp-2], word);
#endif
        static const uint16_t delta[] = { 0, 1, -2, -1 };

        if ((vm->status & ST_IRQ) && (vm->status & ST_IMK)) {
#ifdef DEBUG
                DBG_PRINT("IRQ    (%04x)\n",ARG_OP(vm->t_ext));
#endif
#ifdef UNDER_OVER
                if (vm->rp == vm->ds_size) {
#ifdef DEBUG
                        DBG_PRINT("RC_RS_OVER_FLOW\n");
#endif
                        return RC_RS_OVER_FLOW;
                }
#endif
                vm->rs[++vm->rp] = vm->pc + 1;
                vm->pc = ARG_OP(vm->t_ext);
                vm->status &= ~ST_IRQ;
                return RC_OK;
        }

        if (word & OP_LIT) {
#ifdef DEBUG
                DBG_PRINT("OP_LIT (%04x)\n",ARG_LIT(word));
#endif
#ifdef UNDER_OVER
                if (vm->dp == vm->ds_size) {
#ifdef DEBUG
                        DBG_PRINT("RC_DS_OVER_FLOW\n");
#endif
                        return RC_DS_OVER_FLOW;
                }
                if (vm->pc == vm->RAM_size) {
#ifdef DEBUG
                        DBG_PRINT("RC_PC_OVER_FLOW\n");
#endif
                        return RC_PC_OVER_FLOW;
                }
#endif
                vm->ds[++vm->dp] = vm->t;
                vm->t = ARG_LIT(word);
                vm->pc++;
                return RC_OK;
        }
        switch (OP(word)) {
        case OP_JZ:
#ifdef DEBUG
                DBG_PRINT("OP_JZ  (%04x)\n",ARG(word));
#endif
#ifdef UNDER_OVER
                if (vm->dp == 0) {
#ifdef DEBUG
                        DBG_PRINT("RC_RS_UNDER_FLOW\n");
#endif
                        return RC_RS_UNDER_FLOW;
                }
#endif
                vm->pc = !vm->t ? ARG_OP(word) : vm->pc + 1;
                vm->t  = vm->ds[vm->dp--];
#ifdef UNDER_OVER
                if (vm->pc > vm->RAM_size) {
#ifdef DEBUG
                        DBG_PRINT("RC_PC_OVER_FLOW\n");
#endif
                        return RC_PC_OVER_FLOW;
                }
#endif
                break;
        case OP_JMP:
#ifdef DEBUG
                DBG_PRINT("OP_JMP (%04x)\n",ARG(word));
#endif
                vm->pc = ARG_OP(word);
                break;
        case OP_CALL:
#ifdef DEBUG
                DBG_PRINT("OP_CALL(%04x)\n",ARG_OP(word));
#endif
#ifdef UNDER_OVER
                if (vm->rp == vm->rs_size) {
#ifdef DEBUG
                        DBG_PRINT("RC_RS_OVER_FLOW\n");
#endif
                        return RC_RS_OVER_FLOW;
                }
#endif
                vm->rs[++vm->rp] = (vm->pc +1) << 1;
                vm->pc = ARG_OP(word);
                break;
        case OP_ALU: {
#ifdef DEBUG
                DBG_PRINT("OP_ALU (");
#endif
                uint32_t d;
                uint16_t t, n, r, alu_result;
                t = vm->t;
                n = vm->ds[vm->dp];
                r = vm->rs[vm->rp];
                alu_result = t;
#ifdef DEBUG
                uint8_t r2p = 0;
#endif
                if (word & ALU_F_R2P) {
                        vm->pc = vm->rs[vm->rp] >> 1;
#ifdef DEBUG
                        r2p = 1;
#endif
                } else {
                        vm->pc++;
                }

                switch (ALU_OP(word)) {
                case ALU_OP_T:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_T) ");
#endif
                        break;
                case ALU_OP_N:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_N) ");
#endif
                        alu_result = n;
                        break;
                case ALU_OP_R:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_R) ");
#endif
                        alu_result = r;
                        break;
                case ALU_OP_GET:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_GET)  ");
#endif
                        alu_result = sm1_mem_get(t>>1, vm);
                        break;
                case ALU_OP_PUT:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_PUT) ");
#endif
                        sm1_mem_put(t>>1, n, vm);
                        alu_result = vm->ds[--vm->dp];
                        break; //TODO Check
                case ALU_OP_DPLUS:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_DPLUS) ");
#endif
                        d = (uint32_t)t + n;
                        alu_result = d >> 16;
                        vm->ds[vm->dp] = d;
                        n = d;
                        break;
                case ALU_OP_DMUL:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_DMUL) ");
#endif
                        d = (uint32_t) t * n;
                        n = d >> 16;
                        vm->ds[vm->dp] = d;
                        alu_result = d;
                        break;
                case ALU_OP_AND:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_AND) ");
#endif
                        alu_result &= n;
                        break;
                case ALU_OP_OR:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_OR) ");
#endif
                        alu_result |= n;
                        break;
                case ALU_OP_XOR:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_XOR) ");
#endif
                        alu_result ^= n;
                        break;
                case ALU_OP_NEG:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_NEG) ");
#endif
                        alu_result = ~t;
                        break;
                case ALU_OP_DEC:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_DEC) ");
#endif
                        --alu_result;
                        break;
                case ALU_OP_EQ0:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_EQ0) ");
#endif
                        alu_result = -(t == 0);
                        break;
                case ALU_OP_EQ:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_EQ) ");
#endif
                        alu_result = -(t == n);
                        break;
                case ALU_OP_UCMP:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_UCMP) ");
#endif
                        alu_result = -(n < t);
                        break; //TODO Check
                case ALU_OP_CMP:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_CMP) ");
#endif
                        alu_result = -((int16_t)n < (int16_t)t);
                        break; //TODO Check
                case ALU_OP_RSHIFT:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_RSHIFT) ");
#endif
                        alu_result = (n >> t);
                        break;
                case ALU_OP_LSHIFT:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_LSHIFT) ");
#endif
                        alu_result = (n << t);
                        break;
                case ALU_OP_SP:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_SP) ");
#endif
                        alu_result = vm->dp << 1;
                        break;
                case ALU_OP_RS:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_RS) ");
#endif
                        alu_result = vm->rp << 1;
                        break;
                case ALU_OP_SETSP:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_SETSP) ");
#endif
                        vm->dp = t >> 1;
                        break;
                case ALU_OP_SETRP:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_RP) ");
#endif
                        vm->rp = t >> 1;
                        break;
                case ALU_OP_ST:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_ST) ");
#endif
                        alu_result = vm->status;
                        break;
                case ALU_OP_TX:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_TX) ");
#endif
                        vm->t_ext = t;
                        vm->n_ext = n;
                        vm->status |= ST_SNDTN;
                        break; //TODO
                case ALU_OP_RX:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_RX) ");
#endif
                        if (!(vm->status & ST_RCVTN))
                                break;
                        alu_result = vm->t_ext;
                        break;
                case ALU_OP_UMOD:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_UMOD) ");
#endif
                        if (t) {
                        	    d = vm->ds[--vm->dp]|((uint32_t)n<<16);
                                alu_result = d / t;
                                t = d % t;
                                n = t;
                        } else {
                               //TODO Exception
                        }
                        break;
                case ALU_OP_MOD:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_MOD) ");
#endif
                        if (t) {
                                alu_result = (int16_t)n / t;
                                t = (int16_t)n % t;
                                n = t;
                        } else {
                               //TODO Exception
                        }
                        break; //TODO Check
                case ALU_OP_BYE:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_BYE) ");
#endif
                        return RC_BYE;
                        break;
                case ALU_OP_SETST:
#ifdef DEBUG
                        DBG_PRINT("ALU_OP_SETST) ");
#endif
                        vm->status &= t;
                        break;
                default:
#ifdef DEBUG
                        DBG_PRINT("RC_OP_UNKNOWN\n");
#endif
                        return RC_OP_UNKNOWN;
                }

                vm->dp += delta[ALU_DS(word)];
                vm->rp += delta[ALU_RS(word)];

#ifdef DEBUG
                if(r2p) DBG_PRINT("/ALU_F_R2P ");
#endif
                if (word & ALU_F_N2T) {
#ifdef DEBUG
                        DBG_PRINT("/ALU_F_N2T ");
#endif
                        vm->t = vm->ds[vm->dp];
                }
                if (word & ALU_F_T2R) {
#ifdef DEBUG
                        DBG_PRINT("/ALU_F_T2R ");
#endif
                        vm->rs[vm->rp] = vm->t;
                }
                if (word & ALU_F_T2N) {
#ifdef DEBUG
                        DBG_PRINT("/ALU_F_T2N ");
#endif
                        vm->ds[vm->dp] = vm->t;
                }
#ifdef UNDER_OVER
                if (vm->rp == vm->RAM_size) {
#ifdef DEBUG
                        DBG_PRINT("RC_PC_OVER_FLOW\n");
#endif
                        return RC_PC_OVER_FLOW;
                }
#endif
#ifdef DEBUG
                DBG_PRINT("[delta_dp:%d/delta_rp:%d/alu:%04x]",delta[ALU_DS(word)],delta[ALU_RS(word)],alu_result);
#endif
                vm->t = alu_result;
#ifdef DEBUG
                DBG_PRINT("\n");
#endif
        }
        }

#ifdef UNDER_OVER
        if (vm->rp > vm->rs_size) {
#ifdef DEBUG
                DBG_PRINT("RC_RS_OVER_FLOW_end\n");
#endif
                return RC_RS_OVER_FLOW;
        }
        if (vm->dp > vm->ds_size) {
#ifdef DEBUG
                DBG_PRINT("RC_DS_OVER_FLOW_end\n");
#endif
                return RC_DS_OVER_FLOW;
        }
        if (vm->dp < 0) {
#ifdef DEBUG
                DBG_PRINT("RC_DS_UNDER_FLOW_end\n");
#endif
                return RC_DS_UNDER_FLOW;
        }
        if (vm->rp < 0) {
#ifdef DEBUG
                DBG_PRINT("RC_RS_UNDER_FLOW_end\n");
#endif
                return RC_RS_UNDER_FLOW;
        }
#endif
        return RC_OK;
}

#endif /* __SM1_H__ */
