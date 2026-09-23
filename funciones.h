#include "funciones.c"


typedef struct{
    int base;
    int tamanio;
}TablaSeg;

typedef struct{
   unsigned char memoria[16384];
   TablaSeg TDS[8];
   unsigned int registros[32];
}MV;

// todas las funciones teniendo en cuenta que si no tiene operando es stop si tiene dos operandos son algunas funciones y si tiene un operando son otr
void actualizarCC (MV *maquina,  int valor);
//Si es operacion con dos operandos 
void MOV (MV *maquina,unsigned char opA, unsigned char opB);


void ADD (MV *maquina,unsigned char oA, unsigned char opB);
void SUB (MV *maquina,unsigned char opA, unsigned char opB);
void MUL (MV *maquina,unsigned char opA, unsigned char opB);
void DIV (MV *maquina,unsigned char opA, unsigned char opB);
void CMP (MV *maquina,unsigned char opA, unsigned char opB);
void AND (MV *maquina,unsigned char opA, unsigned char opB);
void OR (MV *maquina,unsigned char opA, unsigned char opB);
void XOR (MV *maquina,unsigned char opA, unsigned char opB);
void SWAP (MV *maquina,unsigned char opA, unsigned char opB);
void SHL (MV *maquina,unsigned char opA, unsigned char opB);
void SHR (MV *maquina,unsigned char opA, unsigned char opB);
void SAR (MV *maquina,unsigned char opA, unsigned char opB);
void LDH (MV *maquina,unsigned char opA, unsigned char opB);
void LDL (MV *maquina,unsigned char opA, unsigned char opB);
void RND (MV *maquina,unsigned char opA, unsigned char opB);

//un solo operando
void SYS (MV *maquina,unsigned char opB);
void JP (MV *maquina,unsigned char opB);
void JN (MV *maquina,unsigned char opB);
void JZ (MV *maquina,unsigned char opB);
void JC (MV *maquina,unsigned char opB);
void JV (MV *maquina,unsigned char opB);
void JNP (MV *maquina,unsigned char opB);
void JNN (MV *maquina,unsigned char opB);
void JNZ (MV *maquina,unsigned char opB);
void NOT (MV *maquina,unsigned char opB);


//sin operandos
void STOP(MV *maquina);

