#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"
#include "OPCodigos.h"

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

void leeArchivo(MV *maquina, char *nombreArch){
    FILE *archb;
    char encabezado[8];

    archb = fopen(nombreArch, "rb");
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


int leerOp(MV *maquina,int tipo,int *pos){
    int valor= tipo<<24;
    if(tipo==1){
        valor |= maquina->memoria[*pos];
        (*pos)++;}
    else
        if(tipo==2){
            valor |= (maquina->memoria[*pos]<<8) | maquina->memoria[*pos+1];
            (*pos)+=2;}
        else
            if(tipo==3){
                valor |= (maquina->memoria[*pos]<<16) | (maquina->memoria[*pos+1]<<8) | maquina->memoria[*pos+2];
                (*pos)+=3;}
    return valor;
}

Instruccion decodificar(MV *maquina, unsigned int direcFisica){
    Instruccion instr = {0};
    unsigned char byte = maquina->memoria[direcFisica];
    int pos = direcFisica + 1;

    instr.opc = byte & 0x1F;
    instr.cantOperandos = tablaOPC[instr.opc].cantOperandos;

    if (instr.cantOperandos == 2){
        instr.tipoB = (byte >> 6) & 0x03;
        instr.tipoA = (byte >> 4) & 0x03;
        instr.opB = leerOp(maquina, instr.tipoB, &pos);
        instr.opA = leerOp(maquina, instr.tipoA, &pos);
    } else if (instr.cantOperandos == 1){
        instr.tipoA = (byte >> 6) & 0x03;
        instr.opA = leerOp(maquina, instr.tipoA, &pos);
    }

    instr.longitud = pos - direcFisica;
    return instr;
}



void ejecutarInstruccion(MV *maquina, int opc, unsigned int opA, unsigned int opB){
    if (tablaOPC[opc].mnemonico == NULL){
        printf("Error: instruccion invalida\n");
        return;
    }
    tablaOPC[opc].ejecutar(maquina, opA, opB);
}


void ejecucion(MV *maquina){
    
    while (maquina->registros[0] < maquina->TDS[0].tamanio){
        
        unsigned int IP = maquina->registros[0];
        Instruccion instr = decodificar(maquina, IP);

        maquina->registros[1] = instr.opc;
        maquina->registros[2] = instr.opA;
        maquina->registros[3] = instr.opB;
        maquina->registros[0] = IP + instr.longitud;

        ejecutarInstruccion(maquina, instr.opc, instr.opA, instr.opB);
    }
}        
void Disassembler(MV *maquina){
    int direccion = 0 ;
    while (direccion < maquina->TDS[0].tamanio){
        Instruccion instr = decodificar(maquina, direccion);
        
        printf("[%04X] ", direccion);
        for( int i=0; i< instr.longitud; i++)
            printf("%02X ", maquina->memoria[direccion + i]);
        for (int i = instr.longitud; i < 7; i++)
            printf("   ");
        
        printf(" | %s ", tablaOPC[instr.opc].mnemonico);
        if (instr.cantOperandos == 2){
                if (instr.tipoA == 1)
                    printf(" %s ", tablaReg[instr.opA & 0x1F]);
                else if (instr.tipoA == 3)
                    printf("[%s + %d]", tablaReg[instr.opA & 0x1F], instr.opA >> 8 & 0xFFFF);
                printf(" , ");
                if (instr.tipoB == 1)
                    printf(" %s ", tablaReg[instr.opB & 0x1F]);
                else if (instr.tipoB == 2)
                    printf(" %d ", instr.opB & 0xFFFF);
                else if (instr.tipoB == 3)
                    printf("[%s + %d]", tablaReg[instr.opB & 0x1F], instr.opB >> 8 & 0xFFFF);
        }
        else if (instr.cantOperandos == 1){
            if (instr.tipoA == 1)
                printf(" %s ", tablaReg[instr.opA & 0x1F]);
            else if (instr.tipoA == 2)
                printf(" %d ", instr.opA & 0xFFFF);
            else if (instr.tipoA == 3)
                printf("[%s + %d]", tablaReg[instr.opA & 0x1F], instr.opA >> 8 & 0xFFFF);
        }
        printf("\n");
        direccion += instr.longitud;
    } 
    
}

int main(int argc, char *argv[]){
    MV maquina;
    leeArchivo(&maquina, argv[1]);
    ejecucion(&maquina);

    if (strcmp(argv[2], "-d")==0)
        Disassembler(&maquina);


return 0;
}