( SM1 base words implemented in assembler )

\ ALU words
: ALU_T      h# 0000 ;  \ T
: ALU_N      h# 0100 ;  \ N
: ALU_R      h# 0200 ;  \ top of return stack
: ALU_GET    h# 0300 ;  \ load from address
: ALU_PUT    h# 0400 ;  \ store to address
: ALU_DPLUS  h# 0500 ;  \ double cell addition
: ALU_DMUL   h# 0600 ;  \ double cell multiply
: ALU_AND    h# 0700 ;  \ bitwise AND
: ALU_OR     h# 0800 ;  \ bitwise OR
: ALU_XOR    h# 0900 ;  \ bitwise XOR
: ALU_NEG    h# 0a00 ;  \ bitwise inversion
: ALU_DEC    h# 0b00 ;  \ decrement
: ALU_EQ0    h# 0c00 ;  \ equal to zero
: ALU_EQ     h# 0e00 ;  \ equality test
: ALU_UCMP   h# 0d00 ;  \ unsigned comparison
: ALU_CMP    h# 0f00 ;  \ signed comparison
: ALU_RSHIFT h# 1000 ;  \ logical right shift
: ALU_LSHIFT h# 1100 ;  \ logical left shift
: ALU_SP     h# 1200 ;  \ depth of stack
: ALU_RS     h# 1300 ;  \ R stack depth
: ALU_SETSP  h# 1400 ;  \ set stack depth
: ALU_SETRP  h# 1500 ;  \ set R stack depth
: ALU_ST     h# 1600 ;  \ get status
: ALU_TX     h# 1700 ;  \ send T and N
: ALU_RX     h# 1800 ;  \ receive T
: ALU_UMOD   h# 1900 ;  \ u/mod
: ALU_MOD    h# 1a00 ;  \ /mod
: ALU_BYE    h# 1b00 ;  \ return
: ALU_SETST  h# 1c00 ;  \ set status

: T2N h# 0080 or ;  \ move T to N
: T2R h# 0040 or ;  \ move T to R
: N2T h# 0020 or ;  \ move N to T
: R2P h# 0010 or ;  \ move R to PC

\ Delta
: d+1      h# 0001 or ;
: d-2      h# 0002 or ;
: d-1      h# 0003 or ;
: r+1      h# 0004 or ;
: r-2      h# 0008 or ;
: r-1      h# 000C or ;

\ Instruction type
: imm       h# 8000 or tcode, ;
: alu       h# 6000 or tcode, ;
: ubranch   h# 0000 or tcode, ;
: 0branch   h# 2000 or tcode, ;
: scall     h# 4000 or tcode, ;

\ Derived words
:: noop     ALU_T                  alu ;
:: dup      ALU_T      T2N     d+1 alu ;
:: over     ALU_N      T2N     d+1 alu ;
:: invert   ALU_NEG                alu ;                              
:: um+      ALU_DPLUS              alu ;                             
:: +        ALU_DPLUS  N2T     d-1 alu ;
:: um*      ALU_DMUL               alu ;
:: *        ALU_DMUL   N2T     d-1 alu ;
:: swap     ALU_N      T2N         alu ;
:: nip      ALU_T              d-1 alu ;
:: drop     ALU_N              d-1 alu ;
:: exit     ALU_T      R2P r-1     alu ;
:: >r       ALU_N      T2R r+1 d-1 alu ;
:: r>       ALU_R      T2N r-1 d+1 alu ;
:: r@       ALU_R      T2N     d+1 alu ;
:: @        ALU_GET                alu ;
:: !        ALU_PUT            d-1 alu ;
:: rshift   ALU_RSHIFT         d-1 alu ;
:: lshift   ALU_LSHIFT         d-1 alu ;
:: =        ALU_EQ             d-1 alu ;
:: u<       ALU_UCMP           d-1 alu ;
:: <        ALU_CMP            d-1 alu ;
:: and      ALU_AND            d-1 alu ;
:: xor      ALU_XOR            d-1 alu ;
:: or       ALU_OR             d-1 alu ;
:: sp@      ALU_SP     T2N     d+1 alu ;
:: sp!      ALU_SETSP              alu ;
:: 1-       ALU_DEC                alu ;
:: rp@      ALU_RS     T2N     d+1 alu ;
:: rp!      ALU_SETSP          d-1 alu ;
:: 0=       ALU_EQ0                alu ;
:: nop      ALU_T                  alu ;
:: (bye)    ALU_BYE                alu ;
:: rx?      ALU_RX     T2N     d+1 alu ;
:: tx!      ALU_TX     N2T     d-1 alu ;
:: um/mod   ALU_UMOD   T2N         alu ;
:: /mod     ALU_MOD    T2N         alu ;
:: /        ALU_MOD            d-1 alu ;
:: mod      ALU_MOD    N2T     d-1 alu ;
:: rdrop    ALU_T          r-1     alu ;
:: io@      ALU_GET                alu ;
:: io!      ALU_PUT                alu ;
:: getst    ALU_ST                 alu ;

\ Other derived
:: 2*       ALU_LSHIFT             alu ;
:: 2/       ALU_RSHIFT             alu ;
:: 2dup<    ALU_CMP    T2N     d+1 alu ;
:: dup>r    ALU_T      T2R r+1     alu ;
:: over+    ALU_DPLUS              alu ;
:: 2dupxor  ALU_OR     T2N     d+1 alu ;
:: overand  ALU_AND                alu ;
:: 2dup+    ALU_DPLUS  T2N     d+1 alu ;
:: over=    ALU_EQ                 alu ;
:: depth    ALU_SP     T2N     d+1 alu ;
