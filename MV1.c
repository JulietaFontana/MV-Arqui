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

void inicializaMV2(MV *maquina, int tamPS, int tamKS, int tamCS, int tamDS, int tamES, int tamSS, int entryPoint){
    int base = 0;
    int indice = 0;
    int tamanios[6] = {tamPS, tamKS, tamCS, tamDS, tamES, tamSS};
    int registrosSeg[6] = {PS, KS, CS, DS, ES, SS};

    for(int i = 0; i < 32; i++){
        maquina->registros[i] = 0;
    }
    for(int i = 0; i < 8; i++){
        maquina->TDS[i].base = 0xFFFF;
        maquina->TDS[i].tamanio = 0xFFFF;
    }
    for(int i = 0; i < 6; i++){
        if(tamanios[i] > 0){
            maquina->TDS[indice].base = base;
            maquina->TDS[indice].tamanio = tamanios[i];
            maquina->registros[registrosSeg[i]] = indice << 16;

            base += tamanios[i];
            indice++;
        }
        else
            maquina->registros[registrosSeg[i]] = 0xFFFFFFFF;
    }
    maquina->registros[IP] = maquina->registros[CS] | entryPoint;
    maquina->registros[SP] = maquina->registros[SS] + tamSS;
}
void inicializaPila(MV *maquina, int cantidadParametros, unsigned int direccionArgv){

    maquina->registros[SP] -= 4;
    accesoamemoria(maquina, maquina->registros[SP], 4, 1, 0xFFFFFFFF);

    maquina->registros[SP] -= 4;
    accesoamemoria(maquina, maquina->registros[SP], 4, 1, cantidadParametros);

    maquina->registros[SP] -= 4;
    accesoamemoria(maquina, maquina->registros[SP], 4, 1, direccionArgv);
}

int leeArchivo(MV *maquina, char *nombreArch, int tamPS){
    FILE *archb;
    char encabezado[18];

    archb = fopen(nombreArch, "rb");
    if(archb == NULL){
        printf("Error al abrir el archivo tipo .vmx\n");
        return;
    }else{
        if(fread(encabezado, sizeof(char), 8, archb) != 8){
            printf("cabecera invalida\n");
            fclose(archb);
            return 0;
        }
        int tamCS = ((unsigned char)encabezado[6] << 8) | (unsigned char)encabezado[7];
        if(strncmp(encabezado, "VMX26", 5) == 0 && encabezado[5]==1){   //VMX V=1
                maquina->version = 1;
                if(tamCS <= maquina->tamMemoria){
                    fread(maquina->memoria, sizeof(char), tamCS, archb);
                    incializaMV1(maquina, tamCS);
                }
                else{
                    printf("ERROR: Memoria insuficiente\n");
                    fclose(archb);
                    return 0;
                }
        }else
            if(strncmp(encabezado, "VMX26", 5) == 0 && encabezado[5]==2){ //VMX V=2
                maquina->version = 2;
                if(fread(encabezado + 8, sizeof(char), 10, archb) != 10){
                    printf("cabecera invalida\n");
                    fclose(archb);
                    return 0;
                }
                int tamDS = ((unsigned char)encabezado[8] << 8) | (unsigned char)encabezado[9];
                int tamES = ((unsigned char)encabezado[10] << 8) | (unsigned char)encabezado[11];
                int tamSS = ((unsigned char)encabezado[12] << 8) | (unsigned char)encabezado[13];
                int tamKS = ((unsigned char)encabezado[14] << 8) | (unsigned char)encabezado[15];
                int entryPoint = ((unsigned char)encabezado[16] << 8) | (unsigned char)encabezado[17];

                if(tamCS == 0 || tamSS == 0 || entryPoint >= tamCS){
                    printf("cabecera invalida\n");
                    fclose(archb);
                    return;
                }
                else if(tamPS + tamKS + tamCS + tamDS + tamES + tamSS <= maquina->tamMemoria){
                    inicializaMV2(maquina, tamPS, tamKS, tamCS, tamDS, tamES, tamSS,entryPoint);
                    int indiceCS = maquina->registros[CS] >> 16;
                    fread(maquina->memoria + maquina->TDS[indiceCS].base, sizeof(unsigned char), tamCS, archb);
                    if(tamKS > 0){
                        int indiceKS = maquina->registros[KS] >> 16;
                        fread(maquina->memoria + maquina->TDS[indiceKS].base, sizeof(unsigned char), tamKS, archb);
                    }
                }else{
                    printf("ERROR: Memoria insuficiente\n");
                    fclose(archb);
                    return 0;
                }
            }else
                if (strncmp(encabezado, "VMI26", 5) == 0 && encabezado[5] == 1) { //VMI 
                    int tamMemoriaVMI  = (((unsigned char)encabezado[6] << 8) | (unsigned char)encabezado[7])* 1024;
                    if(tamMemoriaVMI  > maquina->tamMemoria){
                        printf("ERRROR: Memoria insuficiente\n");
                        fclose(archb);
                        return 0;
                    }else{
                        fread(maquina->registros, sizeof(unsigned int), 32, archb);
                        fread(maquina->TDS, sizeof(TablaSeg), 8, archb);
                        fread(maquina->memoria, sizeof(unsigned char), tamMemoriaVMI , archb);
                    }
                }else {
                    printf("cabecera invalida\n");
                    fclose(archb);
                    return;
                }
        fclose(archb);
        return 1;       
    }
}
unsigned int cargaPS(MV *maquina, char *nombreArch, int argc, char *argv[], int inicioParametros){
    int posicion = 0;
    int cantidad = argc - inicioParametros + 1;
    int direcciones[cantidad];

    direcciones[0] = posicion;
    strcpy((char *)maquina->memoria + posicion, nombreArch);
    posicion += strlen(nombreArch) + 1;
    
    for(int i = inicioParametros; i < argc; i++){
        direcciones[i - inicioParametros + 1] = posicion;

        strcpy((char *)maquina->memoria + posicion, argv[i]);
        posicion += strlen(argv[i]) + 1;
    }
    int inicioArgv = posicion;
    for(int i = 0; i < cantidad; i++){
        unsigned int puntero = maquina->registros[PS] | direcciones[i];
        accesoamemoria(maquina, maquina->registros[PS] | posicion, 4, 1, puntero);
        posicion += 4;
    }
    return maquina->registros[PS] | inicioArgv; //devuelve donde comienza argv
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
    int indiceCS = maquina->registros[CS] >> 16;

    while (maquina->registros[IP] != 0xFFFFFFFF && (maquina->registros[IP] >> 16) == indiceCS &&
      (maquina->registros[IP] & 0xFFFF) < maquina->TDS[indiceCS].tamanio){
        
        unsigned int iP = maquina->registros[IP];
        int dirFisica = direccionFisica(maquina, iP, 1);

        if(dirFisica == -1){
            maquina->registros[IP] = 0xFFFFFFFF;
            return;
        }
        Instruccion instr = decodificar(maquina, dirFisica);
        if (maquina->registros[IP] == 0xFFFFFFFF)
            return;

        maquina->registros[OPC] = instr.opc;
        maquina->registros[OP1] = instr.opA;
        maquina->registros[OP2] = instr.opB;
        maquina->registros[IP] = iP + instr.longitud;

        tablaOPC[instr.opc].ejecutar(maquina);

        if (maquina->salirDebugger)
            return;
        if (maquina->registros[IP] == 0xFFFFFFFF)
            return;
        if (maquina->pasoAPaso && !(instr.opc == 0x00 && obtenerValor(maquina, maquina->registros[OP1]) == 0xF))
            breakpoint(maquina);
        if (maquina->salirDebugger)
            return;
    }
}    

void Disassembler(MV *maquina) {
    TablaSeg cs = maquina->TDS[maquina->registros[CS] >> 16];      // code segment---> numero de entrada busca para saber donde esta en la TDS
    int entry = cs.base + (maquina->registros[IP] & 0xFFFF);       // entry point (física), desde cs.base y el offset lo tiene la IP
    int direccion = cs.base; //DESDE DONDE ARRANCO 
    
    while (direccion < cs.base + cs.tamanio) {

        Instruccion instr = decodificar(maquina, direccion);

        if (maquina->registros[IP] == 0xFFFFFFFF)
            return;

        printf("%c[%04X] ", direccion == entry ? '>' : ' ', direccion);

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

            if (instr.tipoA == 1){
                int reg = instr.opA & 0x1F;
                int sector = (instr.opA >> 5) & 0x03;
                if (reg >= EAX && reg <= EFX && sector == 1)
                    printf("%cL ", tablaReg[reg][1]);          // AL, BL...
                else if (reg >= EAX && reg <= EFX && sector == 2)
                    printf("%cH ", tablaReg[reg][1]);          // AH, BH...
                else if (reg >= EAX && reg <= EFX && sector == 3)
                    printf("%cX ", tablaReg[reg][1]);          // AX, BX...
                else
                    printf("%s ", tablaReg[reg]);              // EAX, DS, SP...
            }

            else if (instr.tipoA == 2)
                printf("%d ", (int16_t)(instr.opA & 0xFFFF));

            else if (instr.tipoA == 3){
                int tam = (instr.opA >> 5) & 0x07;
                if (tam == 1) printf("b");
                else if (tam == 2) printf("w");
                else if (tam == 4) printf("l");
                printf("[%s%+d] ", tablaReg[instr.opA & 0x1F],
                    (int16_t)((instr.opA >> 8) & 0xFFFF));
}
        }

        // Estas líneas deben estar FUERA de los if
        printf("\n");
        direccion += instr.longitud;
    }
}
int main(int argc, char *argv[]){

    int disassembler = 0, cargado;
    MV maquina = {0};
    char *archivoVMX = NULL, *archivoVMI = NULL;
    int cantidadParametros = 0, inicioParametros = 0, tamPS = 0;
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
        else if(strcmp(argv[i], "-p") == 0){
            inicioParametros = i + 1;
            cantidadParametros = argc - inicioParametros;
            break;
        }
    }
    maquina.archivoVMI = archivoVMI;
    if(archivoVMX == NULL && archivoVMI == NULL){
        printf("Error: falta un archivo de entrada\n");
        return 1;
    }

    if(inicioParametros > 0 && archivoVMX != NULL){
        tamPS = strlen(archivoVMX) + 1;

        for(int i = inicioParametros; i < argc; i++){
            tamPS += strlen(argv[i]) + 1;
        }

        tamPS += (cantidadParametros + 1) * 4;
    }

    maquina.memoria = calloc(maquina.tamMemoria, sizeof(unsigned char));

    if(maquina.memoria == NULL){
        printf("Error al reservar memoria\n");
        return 1;
    }

    unsigned int direccionArgv = 0xFFFFFFFF;

    if(archivoVMX != NULL){
        cargado = leeArchivo(&maquina, archivoVMX, tamPS);

        if(cargado && maquina.version == 2){

            if(tamPS > 0){
                direccionArgv = cargaPS(&maquina, archivoVMX, argc, argv, inicioParametros);
            }

            inicializaPila(&maquina, cantidadParametros, direccionArgv);
        }
    }
    else{
        cargado = leeArchivo(&maquina, archivoVMI, 0);
    }

    // Si hubo un error al cargar el archivo, termina el programa
    if(cargado == 0 || maquina.registros[IP] == 0xFFFFFFFF){
        free(maquina.memoria);
        return 1;
    }

    if(disassembler){
        Disassembler(&maquina);
    }
    else{
        ejecucion(&maquina);
        printf("CC = %08X\n", maquina.registros[CC]);
    }

    free(maquina.memoria);
    return 0;
}