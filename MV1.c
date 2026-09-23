#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"

void incializa(MV *maquina, int tamCS){

    maquina->TDS[0].base = 0;
    maquina->TDS[0].tamanio = tamCS;
    maquina->TDS[1].base = tamCS;
    maquina->TDS[1].tamanio = 16384 - tamCS;
    for (int i = 2; i < 8; i++) {
        maquina->TDS[i].base = 0xFFFF;
        maquina->TDS[i].tamanio = 0xFFFF;
    }
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
        if(strncmp(encabezado, "VMX26", 5) != 0 || encabezado[5]!=1 || tamCS > 16384 ){
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
    int tipoA, tipoB,OPC;


    while (maquina->registros[0] < maquina->TDS[0].tamanio){
        //limpio registros
        maquina->registros[2] = 0;
        maquina->registros[3] = 0;
        
        unsigned int IP = maquina->registros[0];
        unsigned char byte = maquina->memoria[IP];
        int pos = IP + 1; // Primer byte después de la cabecera
        
        OPC = byte & 0x1F;
        maquina->registros[1] =  OPC;

        if (OPC >= 0x10 && OPC <= 0x1E){  //2 operandos
            tipoB = (byte >> 6);
            tipoA = (byte >> 4) & 0x03;
            maquina->registros[3] = tipoB << 24;
            maquina->registros[2] = tipoA << 24;
            if (tipoB == 1){
                maquina->registros[3] |= maquina->memoria[pos];
                pos++;
            }else
                if (tipoB == 2) {
                    maquina->registros[3] |= (maquina->memoria[pos] << 8) | maquina->memoria[pos + 1];
                    pos += 2;
                }else
                    if (tipoB == 3){

                    }
            if (tipoA == 1){
                maquina->registros[2] |= maquina->memoria[pos];
                pos++;
            }else   //no va a ser tipo ==2
                if (tipoA == 3){

                }
        }else
            if(OPC == 0x0F) //ningun operando
                pos = -1;   // aca iria stop(maquina)
            else
                if (OPC >= 0x00 && OPC <= 0x08){//un operando 
                    tipoA = (byte>>6);
                    maquina->registros[2] = tipoA << 24;
                    if (tipoA == 1){
                        maquina->registros[2] |= maquina->memoria[pos];
                        pos++;
                    }else
                        if (tipoA == 2) {
                            maquina->registros[2] |= (maquina->memoria[pos] << 8) | maquina->memoria[pos + 1];
                            pos += 2;
                        }else
                            if (tipoA == 3){

                        }

                }else
                    printf("Error: instruccion invalida\n");

        maquina->registros[0] = pos; //avanzo con pos 
        //
    }
}


int main(void){
    MV maquina;
    leeArchivo(&maquina);
    ejecucion(&maquina);

    return 0;
}