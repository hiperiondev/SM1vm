header 1+       : 1+        d# 1 + ;
header cell+    : cell+     d# 2 + ;

header <>       : <>        = invert ;
header >        : >         swap < ;
header 0<       : 0<        d# 0 < ;
header 0>       : 0>        d# 0 > ;
header 0<>      : 0<>       d# 0 <> ;
header u>       : u>        swap u< ;

header emit
: emit ( char -- )
    d# 0
    swap
    tx!
    drop
;

header key?
: key?
    d# 2 
    invert
    getst
    0<>
;

header key
: key
    begin
        key?
    until
    rx?
;
\ \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
: main
    d# 65
    begin
        dup
        emit
        d# 1 +
        dup
        d# 90
        >
    until
    d# 64
    emit
    begin
        key
        emit
    again
;