#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "funciones.h"
#include "OPCodigos.h"

void incializaMV1(MV *maquina, int tamCS){

    maquina->TDS[0].base = 0;
    maquina->TDS[0].tamanio = tamCS;
    maquina->TDS[1].base = tamCS;
    maquina->TDS[1].tamanio = maquina->tamMemoria - tamCS;
    for (int i = 2; i < 8; i++) {
        maquina->TDS[i].base = 0xFFFF;
        maquina->TDS[i].tamanio = 0xFFFF;
    }
    for (int i = 0; i < 32; i++) {
    maquina->registros[i] = 0;
    }
    maquina->registros[CS] = 0x00000000; // CS
    maquina->registros[DS] = 0x00010000; // DS
    maquina->registros[IP] = maquina->registros[CS]; // IP
}


void leeArchivo(MV *maquina, char *nombreArch){
    FILE *archb;
    char encabezado[18];

    archb = fopen(nombreArch, "rb");
    if(archb == NULL)
        printf("Error al abrir el archivo tipo .vmx\n");
    else{
        fread(encabezado,sizeof(char),8,archb);
        int tamCS = ((unsigned char)encabezado[6] << 8) | (unsigned char)encabezado[7];
        if(strncmp(encabezado, "VMX26", 5) == 0 && encabezado[5]==1 && tamCS < 16384 ){
            fread(maquina->memoria, sizeof(char), tamCS, archb);
            incializaMV1(maquina, tamCS);
        }else
            if(strncmp(encabezado, "VMX26", 5) == 0 && encabezado[5]==2){
                if(fread(encabezado + 8, sizeof(char), 10, archb) != 10){
                printf("cabecera invalida\n");
                fclose(archb);
                return;
                }

                int tamDS = ((unsigned char)encabezado[8] << 8) | (unsigned char)encabezado[9];
                int tamES = ((unsigned char)encabezado[10] << 8) | (unsigned char)encabezado[11];
                int tamSS = ((unsigned char)encabezado[12] << 8) | (unsigned char)encabezado[13];
                int tamKS = ((unsigned char)encabezado[14] << 8) | (unsigned char)encabezado[15];
                int entryPoint = ((unsigned char)encabezado[16] << 8) | (unsigned char)encabezado[17];

                int tamPS = 0;
            }else 
                if (strncmp(encabezado, "VMI26", 5) == 0 && encabezado[5] == 1) {

                }else 
                    printf("cabecera invalida\n");
        fclose(archb);
                
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
    if (tablaOPC[instr.opc].mnemonico == NULL){
            printf("Error: instruccion invalida\n");
            maquina->registros[IP] = 0xFFFFFFFF;
    }else{
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
    }
    
    return instr;
}


void ejecucion(MV *maquina){
    while (maquina->registros[IP]< maquina->TDS[0].tamanio){
        
        unsigned int iP = maquina->registros[IP];
        Instruccion instr = decodificar(maquina, iP);
        if (maquina->registros[IP] == 0xFFFFFFFF)
            return;

        maquina->registros[OPC] = instr.opc;
        maquina->registros[OP1] = instr.opA;
        maquina->registros[OP2] = instr.opB;
        maquina->registros[IP] = iP + instr.longitud;

        tablaOPC[instr.opc].ejecutar(maquina);
        if (maquina->registros[IP] == 0xFFFFFFFF)
            return;
    }
}    

void Disassembler(MV *maquina) {
    int direccion = 0;

    while (direccion < maquina->TDS[0].tamanio) {

        Instruccion instr = decodificar(maquina, direccion);

        if (maquina->registros[IP] == 0xFFFFFFFF)
            return;

        printf("[%04X] ", direccion);

        for (int i = 0; i < instr.longitud; i++)
            printf("%02X ", maquina->memoria[direccion + i]);

        for (int i = instr.longitud; i < 7; i++)
            printf("   ");

        printf(" | %s ", tablaOPC[instr.opc].mnemonico);

        if (instr.cantOperandos == 2) {

            if (instr.tipoA == 1)
                printf("%s ", tablaReg[instr.opA & 0x1F]);

            else if (instr.tipoA == 2)
                printf("%d ", (int16_t)(instr.opA & 0xFFFF));

            else if (instr.tipoA == 3)
                printf("[%s + %d]", tablaReg[instr.opA & 0x1F],
                       (int16_t)((instr.opA >> 8) & 0xFFFF));

            printf(" , ");

            if (instr.tipoB == 1)
                printf("%s ", tablaReg[instr.opB & 0x1F]);

            else if (instr.tipoB == 2)
                printf("%d ", (int16_t)(instr.opB & 0xFFFF));

            else if (instr.tipoB == 3)
                printf("[%s + %d]", tablaReg[instr.opB & 0x1F],
                       (int16_t)((instr.opB >> 8) & 0xFFFF));

        } else if (instr.cantOperandos == 1) {

            if (instr.tipoA == 1)
                printf("%s ", tablaReg[instr.opA & 0x1F]);

            else if (instr.tipoA == 2)
                printf("%d ", (int16_t)(instr.opA & 0xFFFF));

            else if (instr.tipoA == 3)
                printf("[%s + %d]", tablaReg[instr.opA & 0x1F],
                       (int16_t)((instr.opA >> 8) & 0xFFFF));
        }

        // Estas líneas deben estar FUERA de los if
        printf("\n");
        direccion += instr.longitud;
    }
}
int main(int argc, char *argv[]){

    int disassembler = 0;
    MV maquina = {0};
    char *archivoVMX = NULL, *archivoVMI = NULL;

    maquina.tamMemoria = 16 * 1024; // Tamaño predeterminado

    for(int i = 1; i < argc; i++){

        if(strcmp(argv[i], "-d") == 0){
            disassembler = 1;
        }
        else if(strncmp(argv[i], "m=", 2) == 0){
            maquina.tamMemoria = atoi(argv[i] + 2) * 1024;
        }
        else if(strstr(argv[i], ".vmx") != NULL){
            archivoVMX = argv[i];
        }
        else if(strstr(argv[i], ".vmi") != NULL){
            archivoVMI = argv[i];
        }
    }

    if(archivoVMX == NULL && archivoVMI == NULL){
        printf("Error: falta un archivo de entrada\n");
        return 1;
    }

    maquina.memoria = calloc(maquina.tamMemoria, sizeof(unsigned char));

    if(maquina.memoria == NULL){
        printf("Error al reservar memoria\n");
        return 1;
    }

    if(archivoVMX != NULL)
        leeArchivo(&maquina, archivoVMX);
    else
        leeArchivo(&maquina, archivoVMI);

    if(disassembler)
        Disassembler(&maquina);
    else{
        ejecucion(&maquina);
        printf("CC = %08X\n", maquina.registros[CC]);
    }

    free(maquina.memoria);

    return 0;
}