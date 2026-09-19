<include stdio.h>
<include stdlib.h>
<include string.h>

typedef struct{
   unsigned char memoria[16384];
}MV;

void leeArchivo(MV *maquina){
    File *archb;
    char encabezado[8];

    archb = fopen("Prueba.vmx", "rb");
    if(archb == NULL)
        printf("Error al abrir el archivo tipo .vmx\n");
    else{
        fread(encabezado,sizeof(char),8,archb);
        if(strncmp(encabezado, "VMX26", 5) != 0 || encabezado[5]!=1){
            printf("cabecera invalida");
            fclose(archb);
        }else{
            int tamCS =  ((unsigned char)encabezado[6] << 8) | (unsigned char)encabezado[7];
            fread(maquina->memoria, sizeof(char), tamCS, archb);
            fclose(archb);
        }

    }
}


void main(){
    MV maquina;

    leeArchivo(&maquina);
}