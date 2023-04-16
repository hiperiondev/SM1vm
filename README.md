 [![License](https://img.shields.io/badge/licence-GPLv3-blue.svg?style=flat)](https://github.com/hiperiondev/SM1vm/blob/master/LICENSE) [![Version](https://img.shields.io/badge/version-v1.0-blue.svg)](https://github.com/hiperiondev/SM1vm/)
# SM1vm
 **Tiny embeddable Stack Machine / Forth Machine**

This project is based on H2 VM but modified for to be more flexible and embeddable, especially on constrained microcontrollers.

## About
The project is based on a self-contained library that provides everything necessary to embed the VM (SM1.h).  

The only exception is the memory access functions (sm1_mem_put and sm1_mem_get) which must be defined by the user. This allows adaptation to the local implementation access to the program memory, both in hardware and in logic.

It is possible to build the VM as a library to be used in other projects. As an example see: <https://github.com/hiperiondev/SM1vm-Gambas3>

Also have a very simple assembler and disassembler.

Many functionalities are defined at the time of compilation, allowing to size the machine according to its use as well as the debug (not necessary in the final implementation).
  
These conditional functions are:  
```
#define DEBUG  
    DEBUG information printing  
#define UNDER_OVER  
    Add control on OVERFLOW and UNDERFLOW of stack pointers and PC  
#define CARRY  
    EXPERIMENTAL: Adds CARRY functions in the addition, subtraction, multiplication and division primitives  
#define EXTBITS  
    EXPERIMENTAL: Uses the -2 bits of the delta of the stack pointers for added functions
```

## Instruction Encoding
```
        +---------------------------------------------------------------+
        | F | E | D | C | B | A | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
        +---------------------------------------------------------------+
        | 1 |                    LITERAL VALUE                          |
        +---------------------------------------------------------------+
        | 0 | 0 | 0 | i |        BRANCH TARGET ADDRESS                  |
        +---------------------------------------------------------------+
        | 0 | 0 | 1 | i |        CONDITIONAL BRANCH TARGET ADDRESS      |
        +--------------------------------------------------------------+
        | 0 | 1 | 0 | i |        CALL TARGET ADDRESS                    |
        +---------------------------------------------------------------+
        | 0 | 1 | 1 |   ALU OPERATION   |T2N|T2R|N2T|R2P| RSTACK| DSTACK|
        +---------------------------------------------------------------+
        | F | E | D | C | B | A | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
        +---------------------------------------------------------------+

        T   : Top of data stack
        N   : Next on data stack
        PC  : Program Counter

        i : indirect from register

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
```

## Instruction Set
```
### BRANCH / LITERAL
 JMP: jump
 JMZ: jump if zero
 CLL: call
 LIT: literal

### ALU INSTRUCTIONS
 TOP: t (ALU_EX == 1 : t++)
 SCN: n
 TRS: top of return stack
 GET: load from address t
 PUT: store n to address t
 DPL: double cell addition
 DML: double cell multiply
 AND: bitwise and
 BOR: bitwise or
 XOR: bitwise xor
 NOT: bitwise inversion
 DEC: decrement
 EQ0: equal to zero
 EQU: equality test
 UCP: unsigned comparison (n-t)
 CMP: signed comparison (n<t)
 RSH: logical right shift
 LSH: logical left shift
 GSP: depth of data stack
 GRS: depth of return stack
 SSP: set data stack depth
 SRP: set return stack depth
 REG: get register t (status t == 0xff)
 SRG: set n on register t (status t == 0xff)
 UMD: u/mod
 MOD: /mod
 NXT: compare top and 2nd element of return stack and put comparison to t. If not eq increment 2nd else drop top and 2nd
 GPC: PC to t
 EXF: execute external function
 LOD: load from stack position (from bottom)
 STR: store to stack position (from bottom)
 MOV: copy register t to register n
 
### ALU MODIFIERS
 T2N: move t to n
 T2R: move t to r
 N2T: move n to t
 R2P: move r to pc

### ALU DELTA
 d+1: dp + 1
 d-1: dp - 1
 d-2: dp - 2 (or extended bit) 
 r+1: rp + 1
 r-1: rp - 1
 r-2: rp - 2 (or extended bit)
```

## Assembler
 The assembler accept these directives:
```
    .equ: Assigns a value to a label.
  .macro: Start of a macro. Takes the name as parameter.
          When the name of the macro is written later in the program, 
          the macro definition is expanded at the place it was used. 
   .endm: End of a macro definition.
.include: Start reading from a specified file.
   .word: Define new mnemonic from complete line. Ex. dup@ get t2n d+1
  .label: Assign the address at this point to label.
.comment: Comment line.
    .org: Set the location counter to an absolute value. Only accept decimal number. 
   .data: Insert uint_16 value. 
 .string: Insert string text.
```
NOTE 1: Directives can't be used inside macro.
NOTE 2: Value $HERE$ represent actual address pointer, can be 
        offsetted ex: .equ label $HERE + 3

## Usage example
```
SM1 [-d|-a] file.in [file.out] [basewords] [ramSize]
```

_For more examples and usage, please refer to the [Wiki][wiki]._

## Development setup

[TODO]
```sh
[TODO]
```
## Architecture and design
[TODO]
## Release History

* 1.0
    * [TODO]

## Contributing

1. Fork it (<https://github.com/hiperiondev/SM1vm/fork>)
2. Create your feature branch (`git checkout -b feature/fooBar`)
3. Commit your changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a new Pull Request

<!-- Markdown link & img dfn's -->
[wiki]:https://github.com/hiperiondev/SM1vm/wiki/%5B1%5D-SM1vm:-Tiny-embeddable-Stack-Machine---Forth-Machine
