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

#define OP(x)       (x & 0xe000)                                     /* operand */
#define ARG_OP(x)   (x & 0x1fff)                                     /* argument of operand */
#define ARG_LIT(x)  (x & 0x7fff)                                     /* literal */

#define ALU_OP(x)   ((x >> 8) & 0x1F)                                /* alu operation */
#define ALU_DS(x)   (x & 0x03)                                       /* alu data stack */
#define ALU_RS(x)   ((x >> 2) & 0x03)                                /* alu return stack */
#define ALU_EX(x)   ((ALU_DS(x) == 0x03)|((ALU_RS(x) == 0x03) << 1)) /* extended bits */

/////////////////////////////////////////////////////////////////////////////////////
/*
## Instruction Set Encoding

    +---------------------------------------------------------------+
	| F | E | D | C | B | A | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	+---------------------------------------------------------------+
	| 1 |                    LITERAL VALUE                          |
	+---------------------------------------------------------------+
	| 0 | 0 | 0 |            BRANCH TARGET ADDRESS                  |
	+---------------------------------------------------------------+
	| 0 | 0 | 1 |            CONDITIONAL BRANCH TARGET ADDRESS      |
	+---------------------------------------------------------------+
	| 0 | 1 | 0 |            CALL TARGET ADDRESS                    |
	+---------------------------------------------------------------+
	| 0 | 1 | 1 |   ALU OPERATION   |T2N|T2R|N2T|R2P| RSTACK| DSTACK|
	+---------------------------------------------------------------+
	| F | E | D | C | B | A | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
	+---------------------------------------------------------------+

	T   : Top of data stack
	N   : Next on data stack
	PC  : Program Counter

	LITERAL VALUES : push a value onto the data stack
	CONDITIONAL    : BRANCHS pop and test the T
	CALLS          : PC+1 onto the return stack

	T2N : Move T to N
	T2R : Move T to top of return stack
	N2T : Move the new value of T (or D) to N
	R2P : Move top of return stack to PC

	RSTACK and DSTACK :
	00 = 0
	01 = +1
	10 = -1
	11 = extended bit (not modify stack pointers. used for alu extended operations)
	     or -2 (depend on compilation flag EXTBITS)
*/
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
        ALU_OP_T      = 0x00, /* t */
        ALU_OP_N      = 0x01, /* n */
        ALU_OP_R      = 0x02, /* top of return stack */
        ALU_OP_GET    = 0x03, /* load from address t */
        ALU_OP_PUT    = 0x04, /* store n to address t */
        ALU_OP_DPLUS  = 0x05, /* double cell addition */
        ALU_OP_DMUL   = 0x06, /* double cell multiply */
        ALU_OP_AND    = 0x07, /* bitwise and */
        ALU_OP_OR     = 0x08, /* bitwise or */
        ALU_OP_XOR    = 0x09, /* bitwise xor */
        ALU_OP_NEG    = 0x0a, /* bitwise inversion */
        ALU_OP_DEC    = 0x0b, /* decrement */
        ALU_OP_EQ0    = 0x0c, /* equal to zero */
        ALU_OP_EQ     = 0x0d, /* equality test */
        ALU_OP_UCMP   = 0x0e, /* unsigned comparison */
        ALU_OP_CMP    = 0x0f, /* signed comparison */
        ALU_OP_RSHIFT = 0x10, /* logical right shift */
        ALU_OP_LSHIFT = 0x11, /* logical left shift */
        ALU_OP_SP     = 0x12, /* depth of stack */
        ALU_OP_RS     = 0x13, /* r stack depth */
        ALU_OP_SETSP  = 0x14, /* set stack depth */
        ALU_OP_SETRP  = 0x15, /* set r stack depth */
        ALU_OP_ST     = 0x16, /* get status & t */
		ALU_OP_SETST  = 0x17, /* set status or t */
        ALU_OP_TX     = 0x18, /* send t and n */
        ALU_OP_RX     = 0x19, /* receive t */
        ALU_OP_UMOD   = 0x1a, /* u/mod */
        ALU_OP_MOD    = 0x1b, /* /mod */
#ifdef EXTRAREGS
		ALU_OP_REG    = 0x1c, /* get register t */
		ALU_OP_SETREG = 0x1d, /* set n on register t */
#else
		ALU_OP_NOOP   = 0x1c, /* not defined */
		ALU_OP_NOOP_  = 0x1d, /* not defined */
#endif
		ALU_OP_NOOP__ = 0x1e, /* not defined */
		ALU_OP_BYE    = 0x1f  /* return */
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
        RC_IRQ           = 0x09, /* irq execute */
		RC_EXPTN         = 0xFD, /* alu exception */
        RC_ERROR         = 0xFE, /* generic error */
        RC_BYE           = 0xFF  /* exit */
};

// Status
enum {
        ST_SNDTN = 0x01, /* send */
        ST_RCVTN = 0x02, /* receive */
        ST_CARRY = 0x04, /* carry bit */
        ST_IRQ   = 0x08, /* interrupt */
        ST_IMK   = 0x10, /* interrupt mask */
		ST_EXPTN = 0x40, /* alu exception */
		ST_RSVD  = 0x80  /* reserved */
};

// Registers
typedef struct {
        int8_t   dp;       /* data stack pointer */
        int8_t   rp;       /* return stack pointer */
        uint16_t pc;       /* program counter */
        uint16_t t;        /* top of data stack */
        uint16_t t_ext;    /* external top of data stack */
        uint16_t n_ext;    /* external second element of data stack */
        uint16_t status;   /* status register
                            *   0=SNDTN      / vm data transmission
                            *   1=RCVTN      / external data receive
                            *   2=CARRY      / carry or overflow
                            *   3=IRQ        / interrupt (similar to INTR on Intel 8085)
                            *   4=IMK        / interrupt mask
                            *   5=NOT USED   / not defined
                            *   6=EXCEPTION  / alu exception
                            *   7=RESERVED   / reserved
                            *   8=NOT USED   / not defined
                            *   9=NOT USED   / not defined
                            *  10=NOT USED   / not defined
                            *  11=NOT USED   / not defined
                            *  12=NOT USED   / not defined
                            *  13=NOT USED   / not defined
                            *  14=NOT USED   / not defined
                            *  15=NOT USED   / not defined
                            */
#ifdef EXTRAREGS
        uint16_t *reg;     /* register vector */
        uint8_t  reg_size; /* register size */
#endif
        uint16_t *RAM;     /* ram vector */
      //uint16_t *ROM;     /* rom vector */
        uint16_t *rs;      /* return stack vector */
        uint16_t *ds;      /* data stack vector */
#ifdef UNDER_OVER
        uint16_t RAM_size; /* ram size */
      //uint16_t ROM_size; /* rom size */
        uint8_t  ds_size;  /* data stack size */
        uint8_t  rs_size;  /* return stack size */
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
#ifdef EXTBITS
	    static const uint16_t delta[] = { 0, 1, -1, 0 };
#else
	    static const uint16_t delta[] = { 0, 1, -1, -2 };
#endif
        vm->pc++;

////////// IRQ
        if ((vm->status & ST_IRQ) && !(vm->status & ST_IMK)) {
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
                vm->pc           = ARG_OP(vm->t_ext);
                vm->status      &= ~ST_IRQ;
                vm->status      |= ST_IMK;
                return RC_IRQ;
        }

////////// Literal
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
                vm->t            = ARG_LIT(word);
                return RC_OK;
        }

        switch (OP(word)) {
////////// 0Branch
        case OP_JZ:
#ifdef DEBUG
                DBG_PRINT("OP_JZ  (%04x)\n",ARG_OP(word));
#endif
#ifdef UNDER_OVER
                if (vm->dp == 0) {
#ifdef DEBUG
                        DBG_PRINT("RC_RS_UNDER_FLOW\n");
#endif
                        return RC_RS_UNDER_FLOW;
                }
#endif
                vm->pc = !vm->t ? ARG_OP(word) : vm->pc;
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
////////// Jump
        case OP_JMP:
#ifdef DEBUG
                DBG_PRINT("OP_JMP (%04x)\n",ARG_OP(word));
#endif
                vm->pc = ARG_OP(word);
                break;
////////// Call
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
                vm->rs[++vm->rp] = vm->pc << 1;
                vm->pc           = ARG_OP(word);
                break;
        case OP_ALU: {
////////// ALU
#ifdef DEBUG
                DBG_PRINT("OP_ALU (");
#endif
                uint16_t t   = vm->t;
                uint16_t n   = vm->ds[vm->dp];
                uint16_t r   = vm->rs[vm->rp];
                uint16_t alu = t;
                uint32_t aux = 0;
#ifdef DEBUG
                uint8_t r2p = 0;
#endif
                if (word & ALU_F_R2P) {
                	vm->pc = vm->rs[vm->rp] >> 1;
#ifdef DEBUG
                    r2p = 1;
#endif
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
                    alu = n;
                    break;
                case ALU_OP_R:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_R) ");
#endif
                    alu = r;
                    break;
                case ALU_OP_GET:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_GET)  ");
#endif
#ifdef UNDER_OVER

#endif
                    alu = sm1_mem_get(t>>1, vm);
                    break;
                case ALU_OP_PUT:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_PUT) ");
#endif
                    sm1_mem_put(t>>1, n, vm);
                    alu = vm->ds[--vm->dp];
                    break;
                case ALU_OP_DPLUS:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_DPLUS) ");
#endif
                    aux            = (uint32_t)t + n;
#ifdef CARRY
                    aux            += (vm->status & ST_CARRY) >> 2;
                    vm->status     &= ~ST_RSVD;
                    vm->status     |= (vm->status & ST_CARRY) << 5;
                    vm->status     &= ~ST_CARRY;
                    vm->status     |= (uint32_t)(t > 0xffffffff - (n + ((vm->status & ST_CARRY) >> 7)))? ST_CARRY:0;
#endif
                    alu            = aux >> 16;
                    vm->ds[vm->dp] = aux;
                    n              = aux;
                    break;
                case ALU_OP_DMUL:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_DMUL) ");
#endif
                    aux            = (uint32_t) t * n;
                    n              = aux >> 16;
                    vm->ds[vm->dp] = aux;
                    alu            = aux;
#ifdef CARRY
                    vm->status &= ~ST_CARRY;
                    vm->status |= (t != 0 && aux / t != n) << 2;
#endif
                    break;
                case ALU_OP_AND:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_AND) ");
#endif
                    alu &= n;
                    break;
                case ALU_OP_OR:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_OR) ");
#endif
                    alu |= n;
                    break;
                case ALU_OP_XOR:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_XOR) ");
#endif
                    alu ^= n;
                    break;
                case ALU_OP_NEG:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_NEG) ");
#endif
                    alu = ~t;
                    break;
                case ALU_OP_DEC:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_DEC) ");
#endif
                    --alu;
#ifdef CARRY
                    vm->status     &= ~ST_CARRY;
                    vm->status     |= (alu == 0xffff) << 2;
#endif
                    break;
                case ALU_OP_EQ0:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_EQ0) ");
#endif
                    alu = -(t == 0);
                    break;
                case ALU_OP_EQ:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_EQ) ");
#endif
                    alu = -(t == n);
                    break;
                case ALU_OP_UCMP:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_UCMP) ");
#endif
                    alu = -(n < t);
                    break;
                case ALU_OP_CMP:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_CMP) ");
#endif
                    alu = -((int16_t)n < (int16_t)t);
                    break;
                case ALU_OP_RSHIFT:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_RSHIFT) ");
#endif

#ifdef CARRY
                    aux = (vm->status & ST_CARRY) << 13;
                    vm->status &= ~ST_CARRY;
                    vm->status |= (n & 0x0001) << 2;
#endif
                    alu = (n >> t);
#ifdef CARRY
                        alu |= aux;
#endif
                    break;
                case ALU_OP_LSHIFT:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_LSHIFT) ");
#endif
#ifdef CARRY
                    aux = (vm->status & ST_CARRY) >> 2;
                    vm->status &= ~ST_CARRY;
                    vm->status |= (n & 0x8000) >> 13;
#endif
                    alu = (n << t);
#ifdef CARRY
                    alu |= aux;
#endif
                    break;
                case ALU_OP_SP:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_SP) ");
#endif
                    alu = vm->dp << 1;
                    break;
                case ALU_OP_RS:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_RS) ");
#endif
                    alu = vm->rp << 1;
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
                    alu = vm->status & t;
                    break;
                case ALU_OP_TX:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_TX) ");
#endif
                    vm->t_ext   = t;
                    vm->n_ext   = n;
                    vm->status |= ST_SNDTN;
                    break;
                case ALU_OP_RX:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_RX) ");
#endif
                    if (!(vm->status & ST_RCVTN))
                    	break;
                    alu = vm->t_ext;
                    break;
                case ALU_OP_UMOD:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_UMOD) ");
#endif
                   if (t) {
                	   aux = vm->ds[--vm->dp]|((uint32_t)n<<16);
                       alu = aux / t;
                       t   = aux % t;
                       n   = t;
                   } else {
                        vm->status |= ST_EXPTN;
                        return RC_EXPTN;
                   }
                   break;
                case ALU_OP_MOD:
#ifdef DEBUG
                   DBG_PRINT("ALU_OP_MOD) ");
#endif
                   if (t) {
                	   alu = (int16_t)n / t;
                       t   = (int16_t)n % t;
                       n   = t;
                   } else {
                       vm->status |= ST_EXPTN;
                       return RC_EXPTN;
                   }
                   break;
                case ALU_OP_SETST:
#ifdef DEBUG
                   DBG_PRINT("ALU_OP_SETST) ");
#endif
                   vm->status &= n;
                   vm->status |= t;
                   break;
#ifdef EXTRAREGS
                case ALU_OP_REG:
                	if (t > (vm->reg_size-1)){
                		vm->status |= ST_EXPTN;
                		return RC_EXPTN;
                	}
                	alu = vm->reg[t];
                	break;
                case ALU_OP_SETREG:
                	if (t > (vm->reg_size-1)){
                		vm->status |= ST_EXPTN;
                		return RC_EXPTN;
                	}
                	vm->reg[t] = n;
                	break;
#endif
                case ALU_OP_BYE:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_BYE) ");
#endif
                    return RC_BYE;
                    break;
                default:
#ifdef DEBUG
                    DBG_PRINT("RC_OP_UNKNOWN\n");
#endif
                    return RC_OP_UNKNOWN;
                }

                vm->dp += delta[ALU_DS(word)];
                vm->rp += delta[ALU_RS(word)];

                if (word & ALU_F_T2N) {
#ifdef DEBUG
                        DBG_PRINT("/ALU_F_T2N ");
#endif
                        vm->ds[vm->dp] = vm->t;
                }
                if (word & ALU_F_T2R) {
#ifdef DEBUG
                        DBG_PRINT("/ALU_F_T2R ");
#endif
                        vm->rs[vm->rp] = vm->t;
                }
                if (word & ALU_F_N2T) {
#ifdef DEBUG
                        DBG_PRINT("/ALU_F_N2T ");
#endif
                        vm->t = n;
                } else {
                	    vm->t = alu;
                }
#ifdef DEBUG
                if(r2p) DBG_PRINT("/ALU_F_R2P ");
#endif

#ifdef UNDER_OVER
                if (vm->rp == vm->RAM_size) {
#ifdef DEBUG
                        DBG_PRINT("RC_PC_OVER_FLOW\n");
#endif
                        return RC_PC_OVER_FLOW;
                }
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
