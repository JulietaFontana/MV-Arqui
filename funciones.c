#include "funciones.h"
#include <stdio.h>
#include <stdlib.h>

//ACTUALIZA CC
void actualizarCC (MV *maquina, int valor){
    maquina->registros[17] &= 0x00000000;
    if (valor<0)
        maquina->registros[17] |= 0x80000000; // se activa si es negativo 
    if (valor==0)
        maquina->registros[17] |= 0x40000000; // se activa si es cero)
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

int accesoamemoria(MV *maquina, unsigned int direcLogica,int escribir,unsigned int valorEscribir){
    int cantbytes=4;
    maquina->registros[4] = direcLogica; //LAR
    int dirfisica=direccionFisica(maquina,direcLogica);
    maquina->registros[5] = (cantbytes << 24) | dirfisica; //MAR

    if (escribir){
        maquina->registros[6]=valorEscribir;//MBR
        for(int i=0;i<cantbytes;i++) //cantbytes=4
            maquina->memoria[dirfisica+i] = (valorEscribir>> (8*(cantbytes-1-i)))& 0x0FF;
        return 0;
        }       
    else {
        unsigned int dato=0;
        for (int i=0;i< cantbytes;i++)
            dato=(dato<<8) | maquina-> memoria[dirfisica+i];
        maquina->registros[6]=dato; //MBR
        return dato;
        }   
}

int obtenerValor(MV *maquina, unsigned int op){
    int tipo = op >> 24;
    if (tipo == 1){
        int reg = op & 0x1F;
        return maquina->registros[reg];
    }else if (tipo == 2){
        return op & 0xFFFF;
    }else if(tipo == 3){
        int offset = op >> 8 & 0xFFFF;
        int codReg = op & 0x1F;
        int direcLogica = maquina->registros[codReg] + offset;
        return accesoamemoria(maquina, direcLogica, 0,0);
    }
     return 0;
}
void guardarValor(MV *maquina, unsigned int op, unsigned int valor){
    int tipo = op >> 24;

    if (tipo == 1){
        int reg = op & 0x1F;
        maquina->registros[reg] = valor;
    }else if (tipo ==3){
        int offset = op >> 8 & 0xFFFF;
        int codReg = op & 0x1F;
        int direcLogica = maquina->registros[codReg] + offset;
        accesoamemoria(maquina, direcLogica,1,valor);
    }

}

//dos operandos 
void MOV (MV *maquina ){
    int valor = obtenerValor(maquina, maquina->registros[3]);
    long long resultado64= (long long)(unsigned int) valor;
    int resultado= (unsigned int) resultado64; //LO PASO A 32 BITS

    guardarValor(maquina,maquina->registros[2], (unsigned int) resultado);
    actualizarCC(maquina,resultado);

    if ((long long)(unsigned int)resultado != resultado64)
        maquina->registros[17] |= 0x20000000; //ACTIVA CARRY

    if ((long long)(int)resultado != resultado64)
        maquina->registros[17] |= 0x10000000; //ACTIVA OVERFLOW
}


void ADD (MV *maquina ){
    int a = obtenerValor(maquina, maquina->registros[2]); 
    int b = obtenerValor(maquina, maquina->registros[3]);

    long long resultado64= (long long)(unsigned int)a + (long long)(unsigned int)b;
    int resultado= (unsigned int) resultado64; //LO PASO A 32 BITS

    guardarValor(maquina,maquina->registros[2], (unsigned int) resultado);
    actualizarCC(maquina,resultado);

    if ((long long)(unsigned int)resultado != resultado64)
        maquina->registros[17] |= 0x20000000; //ACTIVA CARRY

    if ((long long)(int)resultado != resultado64)
        maquina->registros[17] |= 0x10000000; //ACTIVA OVERFLOW

}


void SUB (MV *maquina ){ //OPERANDO B TENGO QUE SUMARLE UNO Y SUMAR AL RESTO!!
    int valorA = obtenerValor(maquina, maquina->registros[2]);
    int valorB = obtenerValor(maquina, maquina->registros[3]);

    long long resultado64= (long long)(unsigned int)valorA - (long long)(unsigned int)valorB;
    int resultado= (unsigned int) resultado64; //LO PASO A 32 BITS

    guardarValor(maquina,maquina->registros[2], (unsigned int) resultado);
    actualizarCC(maquina,resultado);

    if ((long long)(unsigned int)resultado != resultado64)
        maquina->registros[17] |= 0x20000000; //ACTIVA CARRY

    if ((long long)(int)resultado != resultado64)
        maquina->registros[17] |= 0x10000000; //ACTIVA OVERFLOW

}

void MUL (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[2]);
    int valorB = obtenerValor(maquina, maquina->registros[3]);

    long long resultado64= (long long)(unsigned int)valorA * (long long)(unsigned int)valorB;
    int resultado= (unsigned int) resultado64; //LO PASO A 32 BITS

    guardarValor(maquina,maquina->registros[2], (unsigned int) resultado);
    actualizarCC(maquina,resultado);

    if ((long long)(unsigned int)resultado != resultado64)
        maquina->registros[17] |= 0x20000000; //ACTIVA CARRY

    if ((long long)(int)resultado != resultado64)
        maquina->registros[17] |= 0x10000000; //ACTIVA OVERFLOW

}

void CMP (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[2]);
    int valorB = obtenerValor(maquina, maquina->registros[3]);
    long long resultado64= (long long)(unsigned int)valorA - (long long)(unsigned int)valorB;
    int resultado= (unsigned int) resultado64; //LO PASO A 32 BITS

    actualizarCC(maquina,resultado);

    if ((long long)(unsigned int)resultado != resultado64)
        maquina->registros[17] |= 0x20000000; //ACTIVA CARRY

    if ((long long)(int)resultado != resultado64)
        maquina->registros[17] |= 0x10000000; //ACTIVA OVERFLOW

}
void AND (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[2]);
    int valorB = obtenerValor(maquina, maquina->registros[3]);

    int resultado = valorA & valorB;

    guardarValor(maquina, maquina->registros[2], resultado);

    actualizarCC(maquina, resultado);
}

void OR (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[2]);
    int valorB = obtenerValor(maquina, maquina->registros[3]);

    int resultado = valorA | valorB;

    guardarValor(maquina, maquina->registros[2], resultado);

    actualizarCC(maquina, resultado);
}

void XOR (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[2]);
    int valorB = obtenerValor(maquina, maquina->registros[3]);

    int resultado = valorA ^ valorB;

    guardarValor(maquina, maquina->registros[2], resultado);

    actualizarCC(maquina, resultado);
}

void SWAP (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[2]);
    int valorB = obtenerValor(maquina, maquina->registros[3]);

    guardarValor(maquina, maquina->registros[2], valorB);
    guardarValor(maquina, maquina->registros[3], valorA);

    actualizarCC(maquina, valorB);
}
void SHL (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[2]);
    int valorB = obtenerValor(maquina, maquina->registros[3]);

    int resultado = valorA << valorB;

    guardarValor(maquina, maquina->registros[2], resultado);

    actualizarCC(maquina, resultado);

}
void SHR (MV *maquina ){
    unsigned int valorA = obtenerValor(maquina, maquina->registros[2]);
    int valorB = obtenerValor(maquina, maquina->registros[3]);

    int resultado = valorA >> valorB;

    guardarValor(maquina, maquina->registros[2], resultado);

    actualizarCC(maquina, resultado);

}
void SAR (MV *maquina ){
    unsigned int valorA = (unsigned int) obtenerValor(maquina, maquina->registros[2]);
    int valorB = obtenerValor(maquina, maquina->registros[3]);

    unsigned int resultado = valorA;

    for (int i = 0; i < valorB; i++){

        if (resultado & 0x80000000) // si el bit de signo era 1
            resultado = (resultado >> 1) | 0x80000000;
        else
            resultado = resultado >> 1;
    }
    guardarValor(maquina, maquina->registros[2], resultado);

    actualizarCC(maquina, resultado);
}
void LDH (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[2]);
    int valorB = obtenerValor(maquina, maquina->registros[3]);

    int resultado = (valorB & 0x0000FFFF) << 16 | (valorA & 0x0000FFFF);
    guardarValor(maquina, maquina->registros[2], resultado);
}
void LDL (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[2]);
    int valorB = obtenerValor(maquina, maquina->registros[3]);

    int resultado = (valorA & 0xFFFF0000) | (valorB & 0x0000FFFF);

    guardarValor(maquina, maquina->registros[2], resultado);
}
void RND (MV *maquina ){
    int limite = obtenerValor(maquina, maquina->registros[3]);
    int resultado = rand() % (limite + 1);
    guardarValor(maquina, maquina->registros[2], resultado);
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
    if ( (maquina->registros[17] & 0x40000000) !=0){
        maquina->registros[0]= maquina->registros[2] & 0x0FFF; //IP
    } 

}
void NOT (MV *maquina ){
    int valor = obtenerValor(maquina, maquina->registros[2]);

    int resultado = ~valor;

    guardarValor(maquina, maquina->registros[2], resultado);

    actualizarCC(maquina, resultado);
}

//sin operandos 
void STOP(MV *maquina ) {
    printf("SE EJECUTO STOP"); //cartel para chequear
    maquina->registros[0] = 0xFFFFFFFF;
}
