: >           swap < ;

: main
    d# 65
    begin
        dup
        tx!
        d# 1 +
        dup
        d# 90
        >
    until
    d# 64
    dup
    tx!
;
