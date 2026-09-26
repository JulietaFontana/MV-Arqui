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
int obtenerValor(MV *maquina, unsigned int op);
void guardarValor(MV *maquina, unsigned int op, unsigned int valor);
int accesoamemoria(MV *maquina, unsigned int direcLogica, int escribir, unsigned int valorEscribir);

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