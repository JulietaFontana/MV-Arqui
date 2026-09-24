#ifndef FUNCIONES_H
#define FUNCIONES_H
#include <stdint.h>

typedef struct{
    uint16_t base;
    uint16_t tamanio;
}TablaSeg;

typedef struct{
   unsigned char memoria[16384];
   TablaSeg TDS[8];
   unsigned int registros[32];
}MV;



// todas las funciones teniendo en cuenta que si no tiene operando es stop si tiene dos operandos son algunas funciones y si tiene un operando son otr
void actualizarCC (MV *maquina,  int valor);

//DIRECCION LOGICA A FISICA
int direccionFisica(MV *maquina, unsigned int direccionLogica);


//Si es operacion con dos operandos 
void MOV (MV *maquina,unsigned int opA, unsigned int opB);
void ADD (MV *maquina,unsigned int opA, unsigned int opB);
void SUB (MV *maquina,unsigned int opA, unsigned int opB);
void MUL (MV *maquina,unsigned int opA, unsigned int opB);
void DIV (MV *maquina,unsigned int opA, unsigned int opB);
void CMP (MV *maquina,unsigned int opA, unsigned int opB);
void AND (MV *maquina,unsigned int opA, unsigned int opB);
void OR (MV *maquina,unsigned int opA, unsigned int opB);
void XOR (MV *maquina,unsigned int opA, unsigned int opB);
void SWAP (MV *maquina,unsigned int opA, unsigned int opB);
void SHL (MV *maquina,unsigned int opA, unsigned int opB);
void SHR (MV *maquina,unsigned int opA, unsigned int opB);
void SAR (MV *maquina,unsigned int opA, unsigned int opB);
void LDH (MV *maquina,unsigned int opA, unsigned int opB);
void LDL (MV *maquina,unsigned int opA, unsigned int opB);
void RND (MV *maquina,unsigned int opA, unsigned int opB);

//un solo operando
void SYS (MV *maquina,unsigned int opA,unsigned int opB);
void JMP (MV *maquina,unsigned int opA,unsigned int opB);
void JP (MV *maquina,unsigned int opA,unsigned int opB);
void JN (MV *maquina,unsigned int opA,unsigned int opB);
void JZ (MV *maquina,unsigned int opA,unsigned int opB);
void JC (MV *maquina,unsigned int opA,unsigned int opB);
void JV (MV *maquina,unsigned int opA,unsigned int opB);
void JNP (MV *maquina,unsigned int opA,unsigned int opB);
void JNN (MV *maquina,unsigned int opA,unsigned int opB);
void JNZ (MV *maquina,unsigned int opA,unsigned int opB);
void NOT (MV *maquina,unsigned int opA,unsigned int opB);


//sin operandos
void STOP(MV *maquina,unsigned int opA,unsigned int opB);

#endif