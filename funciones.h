#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <stdint.h>

typedef struct{
    uint16_t base;
    uint16_t tamanio;
}TablaSeg;

typedef struct{
   unsigned char *memoria;  //TIENE QUE SER DINAMICA AHORA PORQUE ES VARIABLE 
   unsigned int tamMemoria;  //en bytes 
   TablaSeg TDS[8];
   unsigned int registros[32];
}MV;

#define IP  0
#define OPC 1
#define OP1 2
#define OP2 3
#define LAR 4
#define MAR 5
#define MBR 6
#define SP  7
#define BP  8
#define EAX 10
#define EBX 11
#define ECX 12
#define EDX 13
#define EEX 14
#define EFX 15
#define AC  16
#define CC  17
#define CS  26
#define DS  27
#define ES  28
#define SS  29
#define KS  30
#define PS  31



// todas las funciones teniendo en cuenta que si no tiene operando es stop si tiene dos operandos son algunas funciones y si tiene un operando son otr
void actualizarCC (MV *maquina,  int valor);

//DIRECCION LOGICA A FISICA
int direccionFisica(MV *maquina, unsigned int direccionLogica,int cantBytes);
    //es parte de la version 2: el operando puede ser 1 byte B, 2 bytes W o 4 bytes L 
int tamanioOperando(unsigned int op);
int obtenerValor(MV *maquina, unsigned int op);
void guardarValor(MV *maquina, unsigned int op, unsigned int valor);
int accesoamemoria(MV *maquina, unsigned int direcLogica,int cantBytes, int escribir, unsigned int valorEscribir);

//Si es operacion con dos operandos 
void MOV (MV *maquina );
void ADD (MV *maquina );
void SUB (MV *maquina );
void MUL (MV *maquina );
void DIV (MV *maquina );
void CMP (MV *maquina );
void AND (MV *maquina );
void OR (MV *maquina );
void XOR (MV *maquina );
void SWAP (MV *maquina );
void SHL (MV *maquina );
void SHR (MV *maquina );
void SAR (MV *maquina );
void LDH (MV *maquina );
void LDL (MV *maquina );
void RND (MV *maquina );

//un solo operando
void SYS (MV *maquina);
void JMP (MV *maquina);
void JP (MV *maquina);
void JN (MV *maquina);
void JZ (MV *maquina);
void JC (MV *maquina);
void JV (MV *maquina);
void JNP (MV *maquina);
void JNN (MV *maquina);
void JNZ (MV *maquina);
void NOT (MV *maquina);


//sin operandos
void STOP(MV *maquina);

#endif