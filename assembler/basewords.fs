( SM1 base words implemented in assembler )

\ ALU Operations
: t      h# 0000 ; ( T = t )
: n      h# 0100 ; ( T = n )
: r      h# 0200 ; ( T = Top of Return Stack )
: [t]    h# 0300 ; ( T = memory[t] )
: n->[t] h# 0400 ; ( memory[t] = n )
: t+n    h# 0500 ; ( n = n+t, T = carry )
: t*n    h# 0600 ; ( n = n*t, T = upper bits of multiplication )
: t&n    h# 0700 ; ( T = T and N )
: t|n    h# 0800 ; ( T = T  or N )
: t^n    h# 0900 ; ( T = T xor N )
: ~t     h# 0A00 ; ( Invert T )
: t-1    h# 0B00 ; ( T == t - 1 )
: t==0   h# 0C00 ; ( T == 0? )
: t==n   h# 0D00 ; ( T = n == t? )
: nu<t   h# 0E00 ; ( T = n < t )
: n<t    h# 0F00 ; ( T = n < t, signed version )
: n>>t   h# 1000 ; ( T = n right shift by t places )
: n<<t   h# 1100 ; ( T = n left  shift by t places )
: sp@    h# 1200 ; ( T = variable stack depth )
: rp@    h# 1300 ; ( T = return stack depth )
: sp!    h# 1400 ; ( set variable stack depth )
: rp!    h# 1500 ; ( set return stack depth )
: getst  h# 1600 ; ( Set status & T )
: tx     h# 1700 ; ( Transmit Byte: t = byte, T' = error )
: rx     h# 1800 ; ( Block until byte received, T = byte/error )
: um/mod h# 1900 ; ( Remainder/Divide: Double Cell )
: /mod   h# 1A00 ; ( Signed Remainder/Divide: Single Cell )
: vmbye  h# 1B00 ; ( Exit Interpreter )
: setst  h# 1C00 ; ( Set status & T )

\ ALU stack delta operations 
: d+1     h# 0001 or ; ( increment variable stack by one )
: d-1     h# 0003 or ; ( decrement variable stack by one )
: d-2     h# 0002 or ; ( decrement variable stack by two )
: r+1     h# 0004 or ; ( increment variable stack by one )
: r-1     h# 000C or ; ( decrement variable stack by one )
: r-2     h# 0008 or ; ( decrement variable stack by two )

\ Alu flags.
: r->pc   h# 0010 or ; ( Set Program Counter to Top of Return Stack )
: n->t    h# 0020 or ; ( Set Top of Variable Stack to Next on Variable Stack )
: t->r    h# 0040 or ; ( Set Top of Return Stack to Top on Variable Stack )
: t->n    h# 0080 or ; ( Set Next on Variable Stack to Top on Variable Stack )

\ CPU Instructions
: imm       h# 8000 or tw, ;
: alu       h# 6000 or tw, ;
: ubranch   h# 0000 or tw, ;
: 0branch   h# 2000 or tw, ;
: scall     h# 4000 or tw, ;

\ Derived primitives
:: exit     t      r->pc    r-1 alu ;
:: nop      t                   alu ;
:: dup      t      t->n d+1     alu ;
:: over     n      t->n d+1     alu ;
:: invert   ~t                  alu ;
:: um+      t+n                 alu ;
:: +        t+n    n->t d-1     alu ;
:: um*      t*n                 alu ;
:: *        t*n    n->t d-1     alu ;
:: swap     n      t->n         alu ;
:: nip      t           d-1     alu ;
:: drop     n           d-1     alu ;
:: >r       n      t->r d-1 r+1 alu ;
:: r>       r      t->n d+1 r-1 alu ;
:: r@       r      t->n d+1     alu ;
:: @        [t]                 alu ;
:: !        n->[t]      d-1     alu ;
:: rshift   n>>t        d-1     alu ;
:: lshift   n<<t        d-1     alu ;
:: =        t==n        d-1     alu ;
:: u<       nu<t        d-1     alu ;
:: <        n<t         d-1     alu ;
:: and      t&n         d-1     alu ;
:: xor      t^n         d-1     alu ;
:: or       t|n         d-1     alu ;
:: sp@      sp@    t->n d+1     alu ;
:: sp!      sp!                 alu ;
:: 1-       t-1                 alu ;
:: rp@      rp@    t->n d+1     alu ;
:: rp!      rp!         d-1     alu ;
:: 0=       t==0                alu ;
:: yield?   vmbye               alu ;
:: rx?      rx     t->n d+1     alu ;
:: tx!      tx     n->t d-1     alu ;
:: (getst)  getst               alu ;
:: (setst)  setst       d-1     alu ;
:: um/mod   um/mod t->n         alu ;
:: /mod     /mod   t->n         alu ;
:: /        /mod        d-1     alu ;
:: mod      /mod   n->t d-1     alu ;
:: rdrop    t               r-1 alu ;

\ Some words can be implemented in a single instruction which have no
\ analogue within Forth.
:: dup@     [t]    t->n d+1     alu ;
:: dup0=    t==0   t->n d+1     alu ;
:: dup>r    t      t->r     r+1 alu ;
:: 2dup=    t==n   t->n d+1     alu ;
:: 2dupxor  t^n    t->n d+1     alu ;
:: 2dup<    n<t    t->n d+1     alu ;
:: rxchg    r      t->r         alu ;
:: over-and t&n                 alu ;
:: over-xor t^n                 alu ;
