#include "funciones.h"
#include <stdio.h>

//ACTUALIZA CC
void actualizarCC (MV *maquina, int valor){
    maquina->registros[17] &= 0x0000000;
    if (valor<0)
        maquina->registros[17] |= 0x80000000; // se activa si es negativo 
    if (valor==0)
        maquina->registros[17] |= 0x40000000; // se activa si es cero)
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

void memoria(MV *maquina, unsigned int direcLogica){
    maquina->registros[4] = direcLogica; //LAR
    maquina->registros[5] = (4 << 24) | direccionFisica(maquina,direcLogica); //MAR
   //maquina->registros[6] = MBR? Lee o escribe? derecha o izq
}

int obtenerValor(MV *maquina, unsigned int op){
    int tipo = op >> 24;
    if (tipo == 1){
        int reg = op & 0x1F;
        return maquina->registros[reg];
    }else if (tipo == 2){
        return op & 0xFFFF;
    }else if(tipo == 3){
        int offset = op >> 8;
        int codReg = op & 0x1F;
        int direcLogica = maquina->registros[codReg] + offset;
        return accesoamemoria(maquina, direcLogica, 0,0);
    }
}
void guardarValor(MV *maquina, unsigned int op, unsigned int valor){
    int tipo = op >> 24;

    if (tipo == 1){
        int reg = op & 0x1F;
        maquina->registros[reg] = valor;
    }else if (tipo ==3){
        int offset = op >> 8;
        int codReg = op & 0x1F;
        int direcLogica = maquina->registros[codReg] + offset;
        accesoamemoria(maquina, direcLogica,1,valor);
    }

}

//dos operandos 
void MOV (MV *maquina ){
    unsigned int valor = obtenerValor(maquina, maquina->registros[3]);

    guardarValor(maquina, maquina->registros[2], valor);

    actualizarCC(maquina, valor);
}


void ADD (MV *maquina ){



}
void SUB (MV *maquina ){ //OPERANDO B TENGO QUE SUMARLE UNO Y SUMAR AL RESTO!!

}
void MUL (MV *maquina ){

}
void DIV (MV *maquina ){

}
void CMP (MV *maquina ){

}
void AND (MV *maquina ){

}
void OR (MV *maquina ){

}
void XOR (MV *maquina ){

}
void SWAP (MV *maquina ){

}
void SHL (MV *maquina ){

}
void SHR (MV *maquina ){

}
void SAR (MV *maquina ){

}
void LDH (MV *maquina ){

}
void LDL (MV *maquina ){

}
void RND (MV *maquina ){

}

//un operando 
void SYS (MV *maquina ){

}
void JMP (MV *maquina ){ //????
    maquina->registros[0]= maquina->registros[2] & 0x0FFF;
} 

void JP (MV *maquina ){
    if ( (maquina->registros[17] & 0x80000000) == 0){
        maquina->registros[0]= maquina->registros[2] & 0x0FFF; //IP
    } 
}
void JN (MV *maquina ){
    if ( (maquina->registros[17] & 0x80000000) == 0x80000000){
        maquina->registros[0]= maquina->registros[2] & 0x0FFF; //IP
    } 
}
void JZ (MV *maquina ){
    if ((maquina->registros[17] & 0x40000000) == 0x40000000){
        maquina->registros[0]= maquina->registros[2] & 0x0FFF; //IP
    } 

}
void JC (MV *maquina ){
    if ((maquina->registros[17] & 0x20000000) == 0x20000000){
        maquina->registros[0]= maquina->registros[2] & 0x0FFF; //IP
    } 
}
void JV (MV *maquina ){
    if ((maquina->registros[17] & 0x10000000) == 0x10000000){
        maquina->registros[0]= maquina->registros[2] & 0x0FFF; //IP
        } 
}
void JNP (MV *maquina ){
    if ( (maquina->registros[17] & 0x80000000) <=0){
        maquina->registros[0]= maquina->registros[2] & 0x0FFF; //IP
    } 

}
void JNN (MV *maquina ){
    if ( (maquina->registros[17] & 0x40000000) >=0){
        maquina->registros[0]= maquina->registros[2] & 0x0FFF; //IP
    } 

}
void JNZ (MV *maquina ){
    if ( (maquina->registros[17] & 0x80000000) !=0){
        maquina->registros[0]= maquina->registros[2] & 0x0FFF; //IP
    } 

}
void NOT (MV *maquina ){
    unsigned int valor;
    

}

//sin operandos 
void STOP(MV *maquina ) {
    printf("SE EJECUTO STOP"); //cartel para chequear
    maquina->registros[0] = 0xFFFFFFFF;
}
