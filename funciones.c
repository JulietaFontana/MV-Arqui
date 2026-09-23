#include "funciones.h"

//ACTUALIZA CC
void actualizarCC (MV *maquina, int valor){
    maquina->registros[17] &= 0x0000000;
    if (valor<0)
        maquina->registros[17] |= 0x8000000; // se activa si es negativo 
    if (valor==0)
        maquina->registros[17] |= 0x4000000; // se activa si es cero)
    //actualizar c y overflow despues
    
}


//dos operandos 
void MOV (MV *maquina,unsigned char opA, unsigned char opB){

}
void ADD (MV *maquina,unsigned char opA, unsigned char opB){

}
void SUB (MV *maquina,unsigned char opA, unsigned char opB){

}
void MUL (MV *maquina,unsigned char opA, unsigned char opB){

}
void DIV (MV *maquina,unsigned char opA, unsigned char opB){

}
void CMP (MV *maquina,unsigned char opA, unsigned char opB){

}
void AND (MV *maquina,unsigned char opA, unsigned char opB){

}
void OR (MV *maquina,unsigned char opA, unsigned char opB){

}
void XOR (MV *maquina,unsigned char opA, unsigned char opB){

}
void SWAP (MV *maquina,unsigned char opA, unsigned char opB){

}
void SHL (MV *maquina,unsigned char opA, unsigned char opB){

}
void SHR (MV *maquina,unsigned char opA, unsigned char opB){

}
void SAR (MV *maquina,unsigned char opA, unsigned char opB){

}
void LDH (MV *maquina,unsigned char opA, unsigned char opB){

}
void LDL (MV *maquina,unsigned char opA, unsigned char opB){

}
void RND (MV *maquina,unsigned char opA, unsigned char opB){

}

//un operando 
void SYS (MV *maquina,unsigned char opA,unsigned char opB){

}
void JMP (MV *maquina,unsigned char opA,unsigned char opB){
    
}
void JP (MV *maquina,unsigned char opA,unsigned char opB){

}
void JN (MV *maquina,unsigned char opA,unsigned char opB){

}
void JZ (MV *maquina,unsigned char opA,unsigned char opB){

}
void JC (MV *maquina,unsigned char opA,unsigned char opB){

}
void JV (MV *maquina,unsigned char opA,unsigned char opB){

}
void JNP (MV *maquina,unsigned char opA,unsigned char opB){

}
void JNN (MV *maquina,unsigned char opA,unsigned char opB){

}
void JNZ (MV *maquina,unsigned char opA,unsigned char opB){

}
void NOT (MV *maquina,unsigned char opA,unsigned char opB){

}

//sin operandos 
void stop(MV *maquina,unsigned char opA,unsigned char opB) {
    maquina->registros[0] = 0xFFFFFFFF;
}
