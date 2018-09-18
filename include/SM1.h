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
        11 = -2 or
             extended bit (not modify stack pointers. used for alu extended operations)
             (depend on compilation flag EXTBITS)
*/
/////////////////////////////////////////////////////////////////////////////////////

// Operation types
enum {
        OP_JMP  = 0x0000,  /* jump */
        OP_JMZ  = 0x2000,  /* jump if zero */
        OP_CLL  = 0x4000,  /* call */
        OP_ALU  = 0x6000,  /* alu */
        OP_LIT  = 0x8000   /* literal */
};

// ALU flags
enum {
        ALU_F_T2N = 0x80,  /* move T to N */
        ALU_F_T2R = 0x40,  /* move T to R */
        ALU_F_N2T = 0x20,  /* move N to T */
        ALU_F_R2P = 0x10   /* move R to PC */
};

// ALU operations
enum {
        ALU_OP_TOP = 0x00, /* t */
        ALU_OP_SCN = 0x01, /* n */
        ALU_OP_TRS = 0x02, /* top of return stack */
        ALU_OP_GET = 0x03, /* load from address t */
        ALU_OP_PUT = 0x04, /* store n to address t */
        ALU_OP_DPL = 0x05, /* double cell addition */
        ALU_OP_DML = 0x06, /* double cell multiply */
        ALU_OP_AND = 0x07, /* bitwise and */
        ALU_OP_BOR = 0x08, /* bitwise or */
        ALU_OP_XOR = 0x09, /* bitwise xor */
        ALU_OP_NEG = 0x0a, /* bitwise inversion */
        ALU_OP_DEC = 0x0b, /* decrement */
        ALU_OP_EQ0 = 0x0c, /* equal to zero */
        ALU_OP_EQU = 0x0d, /* equality test */
        ALU_OP_UCP = 0x0e, /* unsigned comparison (n-t)*/
        ALU_OP_CMP = 0x0f, /* signed comparison (n<t) */
        ALU_OP_RSH = 0x10, /* logical right shift */
        ALU_OP_LSH = 0x11, /* logical left shift */
        ALU_OP_GSP = 0x12, /* depth of stack */
        ALU_OP_GRS = 0x13, /* r stack depth */
        ALU_OP_SSP = 0x14, /* set stack depth */
        ALU_OP_SRP = 0x15, /* set r stack depth */
        ALU_OP_REG = 0x16, /* get register t (status == 0xff) */
        ALU_OP_SRG = 0x17, /* set n on register t (status == 0xff) */
        ALU_OP_SND = 0x18, /* send t and n */
        ALU_OP_RCV = 0x19, /* receive t */
        ALU_OP_UMD = 0x1a, /* u/mod */
        ALU_OP_MOD = 0x1b, /* /mod */
        ALU_OP_NXT = 0x1c, /* compare top and 2nd element of return stack. If not eq increment 2nd else drop top and 2nd*/
        ALU_OP_GPC = 0x1d, /* PC to t */
        ALU_OP_NP2 = 0x1e, /* not defined */
        ALU_OP_BYE = 0x1f  /* return */
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
        RC_REG_UNKNOWN   = 0x0a, /* unknown register */
        RC_EXPTN         = 0xfd, /* alu exception */
        RC_ERROR         = 0xfe, /* generic error */
        RC_BYE           = 0xff  /* exit */
};

// Status
enum {
        ST_SNDTN    = 0x0001, /* send */
        ST_RCVTN    = 0x0002, /* receive */
        ST_CARRY    = 0x0004, /* carry bit */
        ST_IRQ      = 0x0008, /* interrupt */
        ST_IMK      = 0x0010, /* interrupt mask */
        ST_EXPTN    = 0x0040, /* alu exception */
        ST_RSVD     = 0x0080, /* reserved */
        ST_AUTOINC0 = 0x0100, /* autoincrement register #0 on every read */
        ST_AUTOINC1 = 0x0200, /* autoincrement register #1 on every read */
        ST_AUTOINC2 = 0x0400, /* autoincrement register #2 on every read */
        ST_INDGET   = 0x0800, /* indirect get on register #t */
        ST_INDPUT   = 0x1000  /* indirect put on register #t */
};

// Registers
typedef struct {
         uint8_t dp;          /* data stack pointer */
         uint8_t rp;          /* return stack pointer */
        uint16_t pc;          /* program counter */
        uint16_t t;           /* top of data stack */
        uint16_t t_ext;       /* external top of data stack */
        uint16_t n_ext;       /* external second element of data stack */
        uint16_t status;      /* status register
                               *   0=SND        / vm data transmission
                               *   1=RCV        / external data receive
                               *   2=CARRY      / carry or overflow
                               *   3=IRQ        / interrupt (similar to INTR on Intel 8085)
                               *   4=IMK        / interrupt mask
                               *   5=NOT USED   / not defined
                               *   6=EXCEPTION  / alu exception
                               *   7=RESERVED   / reserved
                               *   8=AUTOINC0   / autoincrement register #0 on every read
                               *   9=AUTOINC1   / autoincrement register #1 on every read
                               *  10=AUTOINC2   / autoincrement register #2 on every read
                               *  11=INDGET     / indirect get on register #t
                               *  12=INDPUT     / indirect put on register #t
                               *  13=NOT USED   / not defined
                               *  14=NOT USED   / not defined
                               *  15=NOT USED   / not defined
                               */
        uint16_t *reg;         /* register vector */
         uint8_t reg_size;     /* register size */
        uint16_t *RAM;         /* ram vector */
        uint16_t *rs;          /* return stack vector */
        uint16_t *ds;          /* data stack vector */
#ifdef UNDER_OVER
        uint16_t RAM_size;     /* ram size */
         uint8_t ds_size;      /* data stack size */
         uint8_t rs_size;      /* return stack size */
#endif

} vm_t;

/////////////////////////////////////////////////////////////////////////////////////

static  uint8_t sm1_mem_put  (uint16_t addr, uint16_t value, vm_t*);
static uint16_t sm1_mem_get  (uint16_t addr, vm_t*);

static inline vm_t* sm1_init(uint16_t ramSize, uint8_t rsSize, uint8_t dsSize,  uint8_t regQty) {
    vm_t* vm =     (vm_t *) malloc(sizeof(vm_t));
    vm->RAM  = (uint16_t *) malloc(sizeof(uint16_t) * ramSize);
    vm->rs   = (uint16_t *) malloc(sizeof(uint16_t) * rsSize);
    vm->ds   = (uint16_t *) malloc(sizeof(uint16_t) * dsSize);
    vm->reg  = (uint16_t *) malloc(sizeof(uint16_t) * regQty);
    vm->reg_size = regQty;
#ifdef UNDER_OVER
    vm->ds_size  = dsSize;
    vm->rs_size  = rsSize;
    vm->RAM_size = ramSize;
#endif

    vm->pc = 0;
    vm->dp = 0;
    vm->rp = 0;

    return vm;
}

static inline uint8_t sm1_step(uint16_t word, vm_t* vm) {
#ifdef DEBUG
            DBG_PRINT("[pc:%04x/dp:%02x/rp:%02x/rs[rp]:%04x/t:%04x/n:%04x/n-1:%04x/n-2:%04x] step(%04x):"
        ,vm->pc, vm->dp, vm->rp, vm->rs[vm->rp], vm->t, vm->ds[vm->dp], vm->ds[vm->dp-1], vm->ds[vm->dp-2], word);
#endif
#ifdef EXTBITS
        static const int8_t delta[] = { 0, 1, -1, 0 };
#else
        static const int8_t delta[] = { 0, 1, -1, -2 };
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
        case OP_JMZ:
#ifdef DEBUG
                DBG_PRINT("OP_JZ  (%04x)\n",ARG_OP(word));
#endif
#ifdef UNDER_OVER
                if (vm->dp == 0) {
#ifdef DEBUG
                        DBG_PRINT("RC_RS_UNDER_FLOW\n");
#endif
                        return RC_DS_UNDER_FLOW;
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
        case OP_CLL:
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
                case ALU_OP_TOP:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_TOP) ");
#endif
                    break;
                case ALU_OP_SCN:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_SCN) ");
#endif
                    alu = n;
                    break;
                case ALU_OP_TRS:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_TRS) ");
#endif
                    alu = r;
                    break;
                case ALU_OP_GET:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_GET)  ");
#endif
#ifdef INDIRECT
                    if (vm->status & ST_INDGET) {
                        if (t > vm->reg_size - 1) return RC_REG_UNKNOWN;
                        aux = t;
                        t = vm->reg[t];
                    }
#endif
#ifdef UNDER_OVER
                    if ((t>>1) > vm->RAM_size) return RC_MEM_OVERFLOW;
#endif
                    alu = sm1_mem_get(t>>1, vm);
#ifdef INDIRECT
#ifdef AUTOINCR
                    if ((t == 0) && (vm->status & ST_AUTOINC0)) vm->reg[t]++;
                    if ((t == 1) && (vm->status & ST_AUTOINC1)) vm->reg[t]++;
                    if ((t == 2) && (vm->status & ST_AUTOINC2)) vm->reg[t]++;
#endif
                    t = aux;
#endif
                    break;
                case ALU_OP_PUT:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_PUT) ");
#endif
#ifdef INDIRECT
                    if (vm->status & ST_INDPUT) {
                        if (t > vm->reg_size - 1) return RC_REG_UNKNOWN;
                        aux = t;
                        t = vm->reg[t];
                    }
#endif
#ifdef UNDER_OVER
                    if ((t>>1) > vm->RAM_size) return RC_MEM_OVERFLOW;
#endif
                    sm1_mem_put(t>>1, n, vm);
                    alu = vm->ds[--vm->dp];
#ifdef INDIRECT
#ifdef AUTOINCR
                    if ((t == 0) && (vm->status & ST_AUTOINC0)) vm->reg[t]++;
                    if ((t == 1) && (vm->status & ST_AUTOINC1)) vm->reg[t]++;
                    if ((t == 2) && (vm->status & ST_AUTOINC2)) vm->reg[t]++;
#endif
                    t = aux;
#endif
                    break;
                case ALU_OP_DPL:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_DPL) ");
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
                case ALU_OP_DML:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_DML) ");
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
                case ALU_OP_BOR:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_BOR) ");
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
                case ALU_OP_EQU:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_EQU) ");
#endif
                    alu = -(t == n);
                    break;
                case ALU_OP_UCP:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_UCP) ");
#endif
                    alu = -(n < t);
                    break;
                case ALU_OP_CMP:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_CMP) ");
#endif
                    alu = -((int16_t)n < (int16_t)t);
                    break;
                case ALU_OP_RSH:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_RSH) ");
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
                case ALU_OP_LSH:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_LSH) ");
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
                case ALU_OP_GSP:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_GSP) ");
#endif
                    alu = vm->dp << 1;
                    break;
                case ALU_OP_GRS:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_GRS) ");
#endif
                    alu = vm->rp << 1;
                    break;
                case ALU_OP_SSP:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_SSP) ");
#endif
                    vm->dp = t >> 1;
                    break;
                case ALU_OP_SRP:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_SRP) ");
#endif
                    vm->rp = t >> 1;
                    break;
                case ALU_OP_SND:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_SND) ");
#endif
                    vm->t_ext   = t;
                    vm->n_ext   = n;
                    vm->status |= ST_SNDTN;
                    break;
                case ALU_OP_RCV:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_RCV) ");
#endif
                    if (!(vm->status & ST_RCVTN))
                        break;
                    alu = vm->t_ext;
                    vm->status &= ~ST_RCVTN;
                    break; //TODO: Receive t and n
                case ALU_OP_UMD:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_UMD) ");
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
                case ALU_OP_REG:
#ifdef DEBUG
                   DBG_PRINT("ALU_OP_REG) ");
#endif
                   if (t == 0xff) {
                        alu = vm->status;
                         break;
                   }
                   if (t > vm->reg_size - 1) return RC_REG_UNKNOWN;
                   alu = vm->reg[t];
#ifdef AUTOINCR
                   if ((t == 0) && (vm->status & ST_AUTOINC0)) vm->reg[t]++;
                   if ((t == 1) && (vm->status & ST_AUTOINC1)) vm->reg[t]++;
                   if ((t == 2) && (vm->status & ST_AUTOINC2)) vm->reg[t]++;
#endif
                   break;
                case ALU_OP_SRG:
#ifdef DEBUG
                   DBG_PRINT("ALU_OP_SRG) ");
#endif
                   if (t == 0xff) {
                        vm->status = t;
                        break;
                   }
                   if (t > vm->reg_size - 1) return RC_REG_UNKNOWN;
                   vm->reg[t] = n;
                   break;
                case ALU_OP_NXT:
#ifdef DEBUG
                   DBG_PRINT("ALU_OP_CRI) ");
#endif
                   alu = -(r == vm->rs[vm->rp+1]);
                   if (alu) {
                       vm->rp -= 2;
                   } else {
                       ++vm->rs[vm->rp + 1];
                   }
                   break;
                case ALU_OP_GPC:
#ifdef DEBUG
                    DBG_PRINT("ALU_OP_GPC) ");
#endif
                    alu = vm->pc;
                    break;
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
#ifdef DEBUG
                DBG_PRINT("[dp:%d|dr:%d] ", delta[ALU_DS(word)], delta[ALU_RS(word)]);
#endif
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
                DBG_PRINT("\nRC_RS_OVER_FLOW_end");
#endif
                return RC_RS_OVER_FLOW;
        }
        if (vm->dp > vm->ds_size) {
#ifdef DEBUG
                DBG_PRINT("\nRC_DS_OVER_FLOW_end\n");
#endif
                return RC_DS_OVER_FLOW;
        }
        if (vm->dp < 0) {
#ifdef DEBUG
                DBG_PRINT("\nRC_DS_UNDER_FLOW_end\n");
#endif
                return RC_DS_UNDER_FLOW;
        }
        if (vm->rp < 0) {
#ifdef DEBUG
                DBG_PRINT("\nRC_RS_UNDER_FLOW_end\n");
#endif
                return RC_RS_UNDER_FLOW;
        }
#endif
        return RC_OK;
}

#endif /* __SM1_H__ */
