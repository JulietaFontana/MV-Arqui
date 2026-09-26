#include "funciones.h"
#include <stdio.h>

//ACTUALIZA CC
void actualizarCC (MV *maquina, int valor){
    maquina->registros[17] &= 0x0000000;
    if (valor<0)
        maquina->registros[17] |= 0x8000000; // se activa si es negativo 
    if (valor==0)
        maquina->registros[17] |= 0x4000000; // se activa si es cero)
    //actualizar c y overflow despues-> caary si el resultado esta bien pero el resultado es mayor a la capacidad que tengo pero si es overflow el resultado esta mal por desbordamiento 
    // la cuenta se almacena en 2 registros en lugar de uno entonces evaluo el otro registro que cae lo que no entra en el primer -> variable de 64 bits! y veo en ese valor si hay carry (> 1111) unsigned int para ver si pierdo algun bit 
    //OVERFLOW: cuenta con signo propaga el primer bit que es de signo y variable de 64bits 
    //CARRY SIN SIGNO Y ME FIJO SI QUEDA ALGO EN 64 Y OVERFLOW PROPAGO EL SIGNO Y COMPARO QUE SEAN IGUALES 
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

//setear MAR Y MB


//dos operandos 
void MOV (MV *maquina,unsigned int opA, unsigned int opB){

}


void ADD (MV *maquina,unsigned int opA, unsigned int opB){



}
void SUB (MV *maquina,unsigned int opA, unsigned int opB){ //OPERANDO B TENGO QUE SUMARLE UNO Y SUMAR AL RESTO!!

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
void JMP (MV *maquina,unsigned int opA,unsigned int opB){ //????
    maquina->registro[0]=opB;
} 

void JP (MV *maquina,unsigned int opA,unsigned int opB){
    if (maquina->registro[17] & 0x80000000 == 0){
        maquina->registro[0]= opB; //IP
    } 
}
void JN (MV *maquina,unsigned int opA,unsigned int opB){
    if (maquina->registro[17] & 0x80000000 == 0x80000000){
        maquina->registro[0]= opB; //IP
    } 
}
void JZ (MV *maquina,unsigned int opA,unsigned int opB){
    if (maquina->registro[17] & 0x40000000 != 0x40000000){
        maquina->registro[0]= opB; //IP
    } 

}
void JC (MV *maquina,unsigned int opA,unsigned int opB){
    if (maquina->registro[17] & 0x20000000 == 0x20000000){
        maquina->registro[0]= opB; //IP
    } 
}
void JV (MV *maquina,unsigned int opA,unsigned int opB){
    if (maquina->registro[17] & 0x10000000 == 0x10000000){
        maquina->registro[0]= opB; //IP
        } 
}
void JNP (MV *maquina,unsigned int opA,unsigned int opB){
    if (maquina->registro[17] & 0x80000000 <=0){
        maquina->registro[0]= opB; //IP
    } 

}
void JNN (MV *maquina,unsigned int opA,unsigned int opB){
    if (maquina->registro[17] & 0x80000000 >=0){
        maquina->registro[0]= opB; //IP
    } 

}
void JNZ (MV *maquina,unsigned int opA,unsigned int opB){
    if (maquina->registro[17] & 0x80000000 !=0){
        maquina->registro[0]= opB; //IP
    } 

}
void NOT (MV *maquina,unsigned int opA,unsigned int opB){
    unsigned int valor;
    

}

//sin operandos 
void STOP(MV *maquina,unsigned int opA,unsigned int opB) {
    maquina->registros[0] = 0xFFFFFFFF;
}
