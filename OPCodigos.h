#ifndef OPC_H
#define OPC_H


#include "funciones.h"

typedef void (*FuncionInstruccion)(MV *maquina, unsigned int opA, unsigned int opB); 

typedef struct {
    const char *mnemonico;
    int cantOperandos; // 0, 1 o 2. -1 si no existe
    FuncionInstruccion ejecutar;
    }  InfoOPC;

typedef struct {
    int opc;
    int cantOperandos;
    unsigned short int tipoA;
    unsigned short int tipoB;
    unsigned int opA;
    unsigned int opB;
    int longitud; // cuántos bytes ocupó la instrucción completa!!!
} Instruccion;

typedef struct {
    int numReg
    char *nombre;
}


InfoOPC    tablaOPC[32]= {
    [0x00]={"SYS",1,SYS},   [0x01]={"JMP",1,JMP}, [0x02]={"JP",1,JP},
    [0x03]={"JN",1,JN},     [0x04]={"JZ",1,JZ},   [0x05]={"JC",1,JC},
    [0x06]={"JV",1,JV},     [0x07]={"JNP",1,JNP}, [0x08]={"JNN",1,JNN},
    [0x09]={"JNZ",1,JNZ},   [0x0A]={"NOT",1,NOT},
    [0x0F]={"STOP",0,STOP},
    [0x10]={"MOV",2,MOV},   [0x11]={"ADD",2,ADD}, [0x12]={"SUB",2,SUB},
    [0x13]={"MUL",2,MUL},   [0x14]={"DIV",2,DIV}, [0x15]={"CMP",2,CMP},
    [0x16]={"AND",2,AND},   [0x17]={"OR",2,OR},   [0x18]={"XOR",2,XOR},
    [0x19]={"SWAP",2,SWAP}, [0x1A]={"SHL",2,SHL}, [0x1B]={"SHR",2,SHR},
    [0x1C]={"SAR",2,SAR},   [0x1D]={"LDL",2,LDL}, [0x1E]={"LDH",2,LDH},
    [0x1F]={"RND",2,RND}
};
#endif

Inforegistos   tablaReg[]={[0]="IP",  [1]="OPC", [2]="OP1", [3]="OP2",
    [4]="LAR", [5]="MAR", [6]="MBR",
    [10]="EAX",[11]="EBX",[12]="ECX",[13]="EDX",[14]="EEX",[15]="EFX",
    [16]="AC", [17]="CC",
    [26]="CS", [27]="DS",};