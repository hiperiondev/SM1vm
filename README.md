 [![License](https://img.shields.io/badge/licence-GPLv3-blue.svg?style=flat)](https://github.com/hiperiondev/SM1vm/blob/master/LICENSE) [![Version](https://img.shields.io/badge/version-v1.0-blue.svg)](https://github.com/hiperiondev/SM1vm/)
# SM1vm
 **Tiny embeddable Stack Machine / Forth Machine**

This project is based on H2 VM but modified for to be more flexible and embeddable, especially on constrained microcontrollers.

## About
The project is based on a self-contained library that provides everything necessary to embed the VM (SM1.h).  

The only exception is the memory access functions (sm1_mem_put and sm1_mem_get) which must be defined by the user. This allows adaptation to the local implementation access to the program memory, both in hardware and in logic.

It is also possible to build the VM as a library to be used in other projects. As an example see: <https://github.com/hiperiondev/SM1vm-Gambas3>

Also have a very simple assembler and disassembler.

Many functionalities are defined at the time of compilation, allowing to size the machine according to its use as well as the debug (not necessary in the final implementation).
  
These conditional functions are:  
```
#define DEBUG  
    DEBUG information printing  
#define UNDER_OVER  
    Add control on OVERFLOW and UNDERFLOW of stack pointers and PC  
#define AUTOINCR
    Define autoincremental registers (0, 1 and 2) on read (and get/put if defined indirect)
#define INDIRECT
    Add indirect address PUT/GET on registers
#define CARRY  
    EXPERIMENTAL: Adds CARRY functions in the addition, subtraction, multiplication and division primitives  
#define EXTBITS  
    EXPERIMENTAL: Uses the -2 bits of the delta of the stack pointers for added functions
```

## Assembler
 The assembler accept these directives:
```
.equ:     Assigns a value to a label. Value $HERE$ represent actual address pointer.
.macro:   Start of a macro. Takes the name as parameter.
          When the name of the macro is written later in the program, 
          the macro definition is expanded at the place it was used. 
          A macro can take up to 10 parameters. 
          These parameters are referred to as @0-@9 within the Macro definition.
          Can't define macro inside macro.
.endm:    End of a macro definition.
.include: Start reading from a specified file.
.word:    Define new mnemonic from complete line. Ex. dup@  get t2n d+1
.label:   Assign the address at this point to label.
.comment: Comment line.
.org:     Set the location counter to an absolute value. Only accept decimal number. 
          If number is preceded by "offset" location counter is incremented by this value. 
.data:    Insert uint_16 value. 
.string:  Insert string text.
```
NOTE: Directives can't be used inside macro.

## FORTH
 **Compiler:
```
[TODO]
```

## Usage example
```
SM1 [-d|-a|-c] file.in [file.out] [basewords] [ramSize]
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
