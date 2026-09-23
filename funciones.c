#include "funciones.h"

//ACTUALIZA CC
void actualizarCC (MV *maquina, int valor){
    maquina->registros[17] &= 0x0000000;
    if (valor<0)
        maquina->registros[17] |= 0x8000000; // se activa si es negativo 
    if (valor==0)
        maquina->registros[17] |= 0x4000000; // se activa si es cero)

}
   