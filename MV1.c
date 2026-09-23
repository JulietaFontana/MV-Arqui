#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"
#include "funciones.c"

void incializa(MV *maquina, int tamCS){

    maquina->TDS[0].base = 0;
    maquina->TDS[0].tamanio = tamCS;
    maquina->TDS[1].base = tamCS;
    maquina->TDS[1].tamanio = 16384 - tamCS;
    for (int i = 2; i < 8; i++) {
        maquina->TDS[i].base = 0xFFFF;
        maquina->TDS[i].tamanio = 0xFFFF;
    }
    //incializo resto de registros?
    for (int i = 0; i < 32; i++) {
    maquina->registros[i] = 0;
    }
    maquina->registros[26] = 0x00000000; // CS
    maquina->registros[27] = 0x00010000; // DS
    maquina->registros[0] = maquina->registros[26]; // IP
}

void leeArchivo(MV *maquina){
    FILE *archb;
    char encabezado[8];

    archb = fopen("Prueba.vmx", "rb");
    if(archb == NULL)
        printf("Error al abrir el archivo tipo .vmx\n");
    else{
        fread(encabezado,sizeof(char),8,archb);
        int tamCS = ((unsigned char)encabezado[6] << 8) | (unsigned char)encabezado[7];
        if(strncmp(encabezado, "VMX26", 5) != 0 || encabezado[5]!=1 || tamCS > 16384 ){ //si tamCS es > 16384????? invalido?
            printf("cabecera invalida"); 
            fclose(archb);
        }else{
            fread(maquina->memoria, sizeof(char), tamCS, archb);
            incializa(maquina, tamCS);
            fclose(archb);
        }

    }
}

void ejecucion(MV *maquina){
    unsigned int IP = maquina->registros[0];
    unsigned char byte = maquina->memoria[IP];

    maquina->registros[1] =  byte & 0x1F; //OPC

    int tipoB = (byte >> 6) & 0x03;
    int tipoA = (byte >> 5) & 0x01;

    int pos = IP + 1; // Primer byte después de la cabecera
    maquina->registros[3] = tipoB << 24; // OP2: guardamos el tipo
    if (tipoB == 2) {
        maquina->registros[3] |= (maquina->memoria[pos] << 8)
                               |
                               
                               
                               maquina->memoria[pos + 1];

        pos += 2;
    }
}


int main(void){
    MV maquina;
    leeArchivo(&maquina);
    ejecucion(&maquina);

    return 0;
}