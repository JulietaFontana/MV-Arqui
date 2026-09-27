    MOV EDX, DS

    LDL ECX, 1
    LDH ECX, 4

    MOV EAX, 0x01
    SYS 0x1

    ADD [0], 5

    MOV EAX, 0x01
    SYS 0x2

    STOP