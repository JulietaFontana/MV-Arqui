<include stdio.h>
<include stdlib.h>
<include string.h>

typedef struct{
   short int base;
   short int tamanio;
}TablaSeg;

typedef struct{
   unsigned char memoria[16384];
   TablaSeg TDS[8];
   unsigned int registros[32];
}MV;


void incializa(MV maquina, int tamCS){

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
        if(strncmp(encabezado, "VMX26", 5) != 0 || encabezado[5]!=1){ //si tamCS es > 16384????? invalido?
            printf("cabecera invalida"); 
            fclose(archb);
        }else{
            int tamCS =  ((unsigned char)encabezado[6] << 8) | (unsigned char)encabezado[7];
            fread(maquina->memoria, sizeof(char), tamCS, archb);
            incializa(maquina, tamCS);
            fclose(archb);
        }

    }
}



void main(){
    MV maquina;

    leeArchivo(&maquina);
}