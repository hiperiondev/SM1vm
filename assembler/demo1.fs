: >           swap < ;

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
    dup
    emit
;
