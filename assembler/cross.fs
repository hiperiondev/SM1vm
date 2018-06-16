( SM1 Cross Compiler )

\   Usage gforth cross.fs <machine.fs> <program.fs>
\
\   Where machine.fs defines the target machine
\   and program.fs is the target program
\

variable lst        \ .lst output file handle

: h#  \ Parse hexadecimal
    base @ >r 16 base !
    0. bl parse >number throw 2drop postpone literal
    r> base ! ; immediate

: tcell     2 ;
: tcells    tcell * ;
: tcell+    tcell + ;

131072 allocate throw constant tflash       \ bytes, target flash
131072 allocate throw constant _tbranches   \ branch targets, cells
tflash      31072 0 fill
_tbranches  131072 0 fill
: tbranches cells _tbranches + ;

variable tdp    0 tdp !
: there     tdp @ ;
: islegal   ;
: tc!       islegal tflash + c! ;
: tc@       islegal tflash + c@ ;
: tw!       islegal tflash + w! ;
: t!        islegal tflash + l! ;
: t@        islegal tflash + uw@ ;
: twalign   tdp @ 1+ -2 and tdp ! ;
: talign    tdp @ 3 + -4 and tdp ! ;
: tc,       there tc! 1 tdp +! ;
: t,        there t!  4 tdp +! ;
: tw,       there tw! tcell tdp +! ;
: org       tdp ! ;

wordlist constant target-wordlist
: add-order ( wid -- ) >r get-order r> swap 1+ set-order ;
: :: get-current >r target-wordlist set-current : r> set-current ;

next-arg included       \ include the machine.fs

( Language basics for target )

warnings on
:: ( postpone ( ;
:: \ postpone \ ;

:: org          org ;
:: include      include ;
:: included     included ;
:: marker       marker ;
:: [if]         postpone [if] ;
:: [else]       postpone [else] ;
:: [then]       postpone [then] ;

: literal
    \ dup $f rshift over $e rshift xor 1 and throw
    dup h# 8000 and if
        \ h# ffff xor recurse
        invert recurse
        alu_neg alu
        cr ." short literal"
    else
        h# 8000 or tw,
        cr ." long literal"
    then
;

( Defining words for target )

: codeptr   tdp @ 2/ dup ." codepointer " . cr ;  \ target data pointer as a jump address

: wordstr ( "name" -- c-addr u )
    >in @ >r bl word count r> >in !
;

variable link 0 link !

:: header
    twalign there
    cr ." link is " link @ .
    link @ tw,
    link !
    bl parse
    dup tc,
    bounds do
        i c@ tc,
    loop
    twalign
;

:: :
    hex
    codeptr s>d
    <# bl hold # # # # #>
    lst @ write-file throw
    wordstr lst @ write-line throw

    create  codeptr ,
    does>   @ scall
;

:: :noname
;

:: ,
    talign
    t,
;

:: allot
    0 ?do
        0 tc,
    loop
;

: shortcut ( orig -- f ) \ insn @orig precedes ;. Shortcut it.
    \ call becomes jump
    dup t@ h# e000 and h# 4000 = if
        dup t@ h# 1fff and over tw!
        true
        cr ." shortcut a "
    else
        dup t@ h# e00c and h# 6000 = if
            dup t@ h# 0010 or r-1 over tw!
            true
            cr ." shortcut b "
        else
            false
        then
    then
    nip
;

:: ;
    there 2 - shortcut      \ true if shortcut applied
    there 0 do
        i tbranches @ there = if
            i tbranches @ shortcut and
        then
    loop
    0= if   \ not all shortcuts worked
       cr ." not all shortcuts worked (evaluate exit)" cr
       s" exit" evaluate
    then
;
:: ;fallthru ;

:: jmp
    ' >body @ ubranch
;

:: constant
    create  ,
    does>   @ literal
;

:: create
    talign
    create there ,
    does>   @ literal
;

( Switching between target and meta )

: target    only target-wordlist add-order definitions ;
: ]         target ;
:: meta     forth definitions ;
:: [        forth definitions ;

: t'        bl parse target-wordlist search-wordlist 0= throw >body @ ;

( eforth's way of handling constants )

: sign>number   ( c-addr1 u1 -- ud2 c-addr2 u2 )
    0. 2swap
    over c@ [char] - = if
        1 /string
        >number
        2swap dnegate 2swap
    else
        >number
    then
;

: base>number   ( caddr u base -- )
    base @ >r base !
    sign>number
    r> base !
    dup 0= if
        2drop drop literal
    else
        1 = swap c@ [char] . = and if
            drop dup literal 32 rshift literal
        else
            -1 abort" bad number"
        then
    then ;
warnings on

:: d# bl parse 10 base>number ;
:: h# bl parse 16 base>number ;
:: ['] ' >body @ 2* literal ;
:: [char] char literal ;

:: asm-0branch
    ' >body @
    0branch
;

( Conditionals )

: resolve ( orig -- )
    there over tbranches ! \ forward reference from orig to this loc
    dup t@ there 2/ or swap tw!
;

:: if
    there
    0 0branch
;

:: then
    resolve
;

:: else
    there
    0 ubranch
    swap resolve
;

:: begin there ;

:: again ( dest -- )
    2/ ubranch
;
:: until
    2/ 0branch
;
:: while
    there
    0 0branch
;
:: repeat
    swap 2/ ubranch
    resolve
;

4 org
: .trim ( a-addr u ) \ shorten string until it ends with '.'
    begin
        2dup + 1- c@ [char] . <>
    while
        1-
    repeat
;
include strings.fs
next-arg 2dup .trim >str constant prefix.
: .suffix  ( c-addr u -- c-addr u ) \ e.g. "bar" -> "foo.bar"
    >str prefix. +str str@
;
: create-output-file w/o create-file throw ;
: out-suffix ( s -- h ) \ Create an output file h with suffix s
    >str
    prefix. +str
    s" ./build/" >str +str str@
    create-output-file
;

:noname
    s" lst" out-suffix lst !
;
execute

target included                         \ include the program.fs

\ [ tdp @ 0 org ] bootloader main [ org ]
[ tdp @ 0 org ] main [ org ]

meta

decimal
0 value file
: dumpall.16
    s" hex" out-suffix to file

    hex
    1024 0 do
        tflash i 2* + w@
        s>d <# # # # # #> file write-line throw
    loop
    file close-file
;

dumpall.16
bye
