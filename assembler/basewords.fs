( SM1 base words implemented in assembler )

\ ALU Operations
: alu_t       h# 0000 ; ( t )
: alu_n       h# 0100 ; ( n )
: alu_r       h# 0200 ; ( top of return stack )
: alu_get     h# 0300 ; ( load from address t )
: alu_put     h# 0400 ; ( store n to address t )
: alu_dplus   h# 0500 ; ( double cell addition )
: alu_dmul    h# 0600 ; ( double cell multiply )
: alu_and     h# 0700 ; ( bitwise and )
: alu_or      h# 0800 ; ( bitwise or )
: alu_xor     h# 0900 ; ( bitwise xor )
: alu_neg     h# 0a00 ; ( bitwise inversion)
: alu_dec     h# 0b00 ; ( decrement )
: alu_eq0     h# 0c00 ; ( equal to zero )
: alu_eq      h# 0d00 ; ( equality test )
: alu_ucmp    h# 0e00 ; ( unsigned comparison )
: alu_cmp     h# 0f00 ; ( signed comparison )
: alu_rshift  h# 1000 ; ( logical right shift )
: alu_lshift  h# 1100 ; ( logical left shift )
: alu_sp      h# 1200 ; ( depth of stack )
: alu_rs      h# 1300 ; ( r stack depth )
: alu_setsp   h# 1400 ; ( set stack depth )
: alu_setrp   h# 1500 ; ( set r stack depth )
: alu_st      h# 1600 ; ( get status & t )
: alu_setst   h# 1700 ; ( set status or t )
: alu_tx      h# 1800 ; ( send t and n )
: alu_rx      h# 1900 ; ( receive t )
: alu_umod    h# 1a00 ; ( u/mod )
: alu_mod     h# 1b00 ; ( /mod )
: alu_reg     h# 1c00 ; ( get register t )
: alu_setreg  h# 1d00 ; ( set n on register t )
: alu_noop__  h# 1e00 ; ( not defined )
: alu_bye     h# 1f00 ; ( return )

\ ALU stack delta operations 
: d+1     h# 0001 or ; ( increment variable stack by one )
: d-1     h# 0002 or ; ( decrement variable stack by one )
: d-2     h# 0003 or ; ( decrement variable stack by two )
: r+1     h# 0004 or ; ( increment variable stack by one )
: r-1     h# 0008 or ; ( decrement variable stack by one )
: r-2     h# 000C or ; ( decrement variable stack by two )

\ ALU flags
: r->pc   h# 0010 or ; ( Set Program Counter to Top of Return Stack )
: n->t    h# 0020 or ; ( Set Top of Variable Stack to Next on Variable Stack )
: t->r    h# 0040 or ; ( Set Top of Return Stack to Top on Variable Stack )
: t->n    h# 0080 or ; ( Set Next on Variable Stack to Top on Variable Stack )

\ CPU Instructions
: imm     h# 8000 or tw, ;
: alu     h# 6000 or tw, ;
: ubranch h# 0000 or tw, ;
: 0branch h# 2000 or tw, ;
: scall   h# 4000 or tw, ;

\ Derived primitives
:: exit     alu_t      r->pc    r-1 alu ;
:: nop      alu_t                   alu ;
:: dup      alu_t      t->n d+1     alu ;
:: over     alu_n      t->n d+1     alu ;
:: invert   alu_neg                 alu ;
:: um+      alu_dplus               alu ;
:: +        alu_dplus  n->t d-1     alu ;
:: um*      alu_dmul                alu ;
:: *        alu_dmul   n->t d-1     alu ;
:: swap     alu_n      t->n         alu ;
:: nip      alu_t           d-1     alu ;
:: drop     alu_n           d-1     alu ;
:: >r       alu_n      t->r d-1 r+1 alu ;
:: r>       alu_r      t->n d+1 r-1 alu ;
:: r@       alu_r      t->n d+1     alu ;
:: @        alu_get                 alu ;
:: !        alu_put         d-1     alu ;
:: rshift   alu_rshift      d-1     alu ;
:: lshift   alu_lshift      d-1     alu ;
:: =        alu_eq          d-1     alu ;
:: u<       alu_ucmp        d-1     alu ;
:: <        alu_cmp         d-1     alu ;
:: and      alu_and         d-1     alu ;
:: xor      alu_xor         d-1     alu ;
:: or       alu_or          d-1     alu ;
:: sp@      alu_sp    t->n  d+1     alu ;
:: sp!      alu_setsp               alu ;
:: 1-       alu_dec                 alu ;
:: rp@      alu_rs    t->n  d+1     alu ;
:: rp!      alu_setrp       d-1     alu ;
:: 0=       alu_eq0                 alu ;
:: yield?   alu_bye                 alu ;
:: rx?      alu_rx     t->n d+1     alu ;
:: tx!      alu_tx     n->t d-1     alu ;
:: getst    alu_st     t->n d+1     alu ;
:: setst    alu_setst       d-1     alu ;
:: um/mod   alu_umod   t->n         alu ;
:: /mod     alu_mod    t->n         alu ;
:: /        alu_mod         d-1     alu ;
:: mod      alu_mod    n->t d-1     alu ;
:: rdrop    alu_t               r-1 alu ;

\ Some words can be implemented in a single instruction which have no
\ analogue within Forth.
:: dup@     alu_get    t->n d+1     alu ;
:: dup0=    alu_eq0    t->n d+1     alu ;
:: dup>r    alu_t      t->r     r+1 alu ;
:: 2dup=    alu_eq     t->n d+1     alu ;
:: 2dupxor  alu_xor    t->n d+1     alu ;
:: 2dup<    alu_cmp    t->n d+1     alu ;
:: rxchg    alu_r      t->r         alu ;
:: over-and alu_and                 alu ;
:: over-xor alu_xor                 alu ;
