	MOV EDX, DS
	LDL ECX,1
	LDH ECX,4
	MOV EAX, 0x01
	SYS 0x1

	ADD EDX, 4 ; muevo 4 luagres de memoria

	STOP