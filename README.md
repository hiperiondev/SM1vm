 [![License](https://img.shields.io/badge/licence-GPLv3-blue.svg?style=flat)](https://github.com/hiperiondev/SM1vm/blob/master/LICENSE) [![Version](https://img.shields.io/badge/version-v1.0-blue.svg)](https://github.com/hiperiondev/SM1vm/)
# SM1vm
 **Tiny embeddable Stack Machine / Forth Machine**

This project is based on H2 VM but modified for to be more flexible and embeddable, especially on constrained microcontrollers.

## About
The project is based on a self-contained library that provides everything necessary to embed the VM.  

The only exception is the memory access functions (sm1_mem_put and sm1_mem_get) which must be defined by the user. This allows adaptation to the local implementation access to the program memory, both in hardware and in logic.

It is also possible to build the VM as a library to be used in other projects. As an example see: <https://github.com/hiperiondev/SM1vm-Gambas3>

Many functionalities are defined at the time of compilation, allowing to size the machine according to its use as well as the debug (not necessary in the final implementation).
  
These conditional functions are:  
```sh
#define DEBUG  
    DEBUG information printing  
#define UNDER_OVER  
    Add control on OVERFLOW and UNDERFLOW of stack pointers, and PC  
#define CARRY  
    EXPERIMENTAL: Adds CARRY functions in the addition, subtraction, multiplition and division primitives  
#define EXTRAREGS  
    Add generic records to the VM (maximum 255)  
#define EXTBITS  
    EXPERIMENTAL: Uses the -2 bits of the delta of the stack pointers for added functions
```

## Usage example

[TODO]

_For more examples and usage, please refer to the [Wiki][wiki]._

## Development setup

[TODO]
```sh
[TODO]
```

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
[wiki]: https://github.com/yourname/yourproject/wiki