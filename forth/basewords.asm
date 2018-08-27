.comment SM1 base words implemented in assembler

.comment Derived primitives
.word exit     top r2p r-1
.word nop      top
.word dup      top t2n d+1
.word over     scn t2n d+1
.word invert   neg
.word um+      dpl
.word +        dpl n2t d-1
.word um*      dml
.word *        dml n2t d-1
.word swap     scn t2n
.word nip      top     d-1
.word drop     scn     d-1
.word >r       scn t2r d-1 r+1
.word r>       trs t2n d+1 r-1
.word r@       trs t2n d+1
.word @        get
.word !        put     d-1
.word rshift   rsh     d-1
.word lshift   lsh     d-1
.word =        equ     d-1
.word u<       ucp     d-1
.word <        cmp     d-1
.word and      and     d-1
.word xor      xor     d-1
.word or       bor     d-1
.word sp@      gsp t2n d+1
.word sp!      ssp
.word 1-       dec
.word rp@      grs t2n d+1
.word rp!      srp     d-1
.word 0=       eq0
.word yield?   bye
.word getrg    reg
.word setrg    srg     d-1
.word um/mod   umd t2n
.word /mod     mod t2n
.word /        mod     d-1
.word mod      mod n2t d-1
.word rdrop    top     r-1
.word depth    gsp t2n d+1
.word next     nxt

.comment Some words can be implemented in a single instruction
.comment which have no analogue within Forth.
.word dup@     get t2n d+1
.word dup0=    eq0 t2n d+1
.word dup>r    top t2r r+1
.word 2dup=    equ t2n d+1
.word 2dupxor  xor t2n d+1
.word 2dup<    cmp t2n d+1
.word rxchg    grt t2r
.word over-and and
.word over-xor xor
.word 2*       lsh
.word 2/       rsh
.word overand  and
.word 2dup+    dpl t2n d+1
.word over=    equ
.word dropr    scn     r-1

.macro execute
       r>
       exit
.endm

.macro @execute
       @ dup if
         execute
       then
 .endm

 .macro -
        invert
        +
 .endm

