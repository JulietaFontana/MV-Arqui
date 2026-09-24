#include "funciones.h"
#include <stdio.h>

//ACTUALIZA CC
void actualizarCC (MV *maquina, int valor){
    maquina->registros[17] &= 0x0000000;
    if (valor<0)
        maquina->registros[17] |= 0x8000000; // se activa si es negativo 
    if (valor==0)
        maquina->registros[17] |= 0x4000000; // se activa si es cero)
    //actualizar c y overflow despues
    
}

//DIRECCION LOGICA A FISICA
int direccionFisica(MV *maquina, unsigned int direccionLogica) {

    unsigned int segmento = direccionLogica >> 16;
    unsigned int desplazamiento = direccionLogica & 0xFFFF;

    if (segmento >= 8) {   // Valida que el segmento exista
        printf("Error: segmento invalido\n");
        return -1;
    }else // Valida que no sea -1
         if (maquina->TDS[segmento].base == 0xFFFF && maquina->TDS[segmento].tamanio == 0xFFFF) {
            printf("Error: segmento no utilizado\n");
            return -1;
        }else
            if (desplazamiento >= maquina->TDS[segmento].tamanio) { // Validar que el acceso esté dentro de sus límites
                printf("Error: direccion fuera del segmento\n");
                return -1;
            }

    return maquina->TDS[segmento].base + desplazamiento;
}


//dos operandos 
void MOV (MV *maquina,unsigned int opA, unsigned int opB){

}


void ADD (MV *maquina,unsigned int opA, unsigned int opB){

}
void SUB (MV *maquina,unsigned int opA, unsigned int opB){

}
void MUL (MV *maquina,unsigned int opA, unsigned int opB){

}
void DIV (MV *maquina,unsigned int opA, unsigned int opB){

}
void CMP (MV *maquina,unsigned int opA, unsigned int opB){

}
void AND (MV *maquina,unsigned int opA, unsigned int opB){

}
void OR (MV *maquina,unsigned int opA, unsigned int opB){

}
void XOR (MV *maquina,unsigned int opA, unsigned int opB){

}
void SWAP (MV *maquina,unsigned int opA, unsigned int opB){

}
void SHL (MV *maquina,unsigned int opA, unsigned int opB){

}
void SHR (MV *maquina,unsigned int opA, unsigned int opB){

}
void SAR (MV *maquina,unsigned int opA, unsigned int opB){

}
void LDH (MV *maquina,unsigned int opA, unsigned int opB){

}
void LDL (MV *maquina,unsigned int opA, unsigned int opB){

}
void RND (MV *maquina,unsigned int opA, unsigned int opB){

}

//un operando 
void SYS (MV *maquina,unsigned int opA,unsigned int opB){

}
void JMP (MV *maquina,unsigned int opA,unsigned int opB){

}
void JP (MV *maquina,unsigned int opA,unsigned int opB){

}
void JN (MV *maquina,unsigned int opA,unsigned int opB){

}
void JZ (MV *maquina,unsigned int opA,unsigned int opB){

}
void JC (MV *maquina,unsigned int opA,unsigned int opB){

}
void JV (MV *maquina,unsigned int opA,unsigned int opB){

}
void JNP (MV *maquina,unsigned int opA,unsigned int opB){

}
void JNN (MV *maquina,unsigned int opA,unsigned int opB){

}
void JNZ (MV *maquina,unsigned int opA,unsigned int opB){

}
void NOT (MV *maquina,unsigned int opA,unsigned int opB){

}

//sin operandos 
void STOP(MV *maquina,unsigned int opA,unsigned int opB) {
    maquina->registros[0] = 0xFFFFFFFF;
}
