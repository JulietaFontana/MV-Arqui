#include "funciones.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>

//ACTUALIZA CC
void actualizarCC (MV *maquina, int valor){
    maquina->registros[CC] &= 0;
    if (valor<0)
        maquina->registros[CC] |= 0x80000000; // se activa si es negativo 
    if (valor==0)
        maquina->registros[CC] |= 0x40000000; // se activa si es cero)
}

//DIRECCION LOGICA A FISICA
int direccionFisica(MV *maquina, unsigned int direccionLogica, int cantBytes) {

    unsigned int segmento = direccionLogica >> 16;
    unsigned int desplazamiento = direccionLogica & 0xFFFF;

    if (segmento >= 8) {   // Valida que el segmento exista
        printf("Error: segmento invalido\n");
        return -1;
    }else // Valida que no sea -1
         if (maquina->TDS[segmento].base == 0xFFFF && maquina->TDS[segmento].tamanio == 0xFFFF) {
            printf("Error: segmento no utilizado\n");
            return -1;
        }else
            if (desplazamiento + cantBytes > maquina->TDS[segmento].tamanio) { // Validar que el acceso esté dentro de sus límites
                printf("Error: direccion fuera del segmento\n");
                return -1;
            }

    return maquina->TDS[segmento].base + desplazamiento;    
}

int accesoamemoria(MV *maquina, unsigned int direcLogica,int cantBytes, int escribir,unsigned int valorEscribir){
    maquina->registros[LAR] = direcLogica; 
    int dirfisica=direccionFisica(maquina,direcLogica,cantBytes);
    if (dirfisica == -1){
        maquina->registros[IP] = 0xFFFFFFFF;
        return -1;
    }
    maquina->registros[MAR] = (cantBytes << 16) | (dirfisica & 0xFFFF); 

    if (escribir){
        maquina->registros[MBR]=valorEscribir;
        for(int i=0;i<cantBytes;i++) //BIG ENDIAN--> se truncan los bytes altos solos
            maquina->memoria[dirfisica+i] = (valorEscribir>> (8*(cantBytes-1-i)))& 0x0FF;
        return 0;
        }       
    else {
        unsigned int dato=0;
        for (int i=0;i< cantBytes;i++)
            dato=(dato<<8) | maquina-> memoria[dirfisica+i];
        
        if (cantBytes < 4) {                  // propagar signo: 0xAB -> 0xFFFFFFAB
            int corrimiento = 32 - 8 * cantBytes;
            dato = (unsigned int)(((int)(dato << corrimiento)) >> corrimiento);
        }
        maquina->registros[MBR] = dato;
        return dato;
}}

//nuevo
int tamanioOperando(unsigned int op){
    int cod = (op >> 5) & 0x07;
    if (cod == 1) return 1;   // b
    if (cod == 2) return 2;   // w
    return 4;                 // l
}
// Lee un registro según su sector, propagando el signo
int leerRegistro(MV *maquina, unsigned int op){
    int reg = op & 0x1F; //los ultimos 5 son el registro
    int sector = (op >> 5) & 0x03; //posicion 6 y 7 me dicen el sector entonces cuantos bytes debo obtener
    unsigned int valor = maquina->registros[reg];

    switch(sector){
        case 1:  return (int8_t)(valor & 0xFF);          // AL--> el int8_t es para decir entero de 8 bits con signo! sino no propaga el signo
        case 2:  return (int8_t)((valor >> 8) & 0xFF);   // AH
        case 3:  return (int16_t)(valor & 0xFFFF);       // AX
        default: return valor;                           // EAX
    }
}

int obtenerValor(MV *maquina, unsigned int op){
    int tipo = op >> 24;
    if (tipo == 1){
        return leerRegistro(maquina, op);
    }else if (tipo == 2){
        return (int16_t)(op & 0xFFFF);
    }else if(tipo == 3){
        int offset = (int16_t)((op >> 8) & 0xFFFF);
        int codReg = op & 0x1F;
        int direcLogica = maquina->registros[codReg] + offset;
        return accesoamemoria(maquina, direcLogica,tamanioOperando(op), 0,0);
    }
     return 0;
}
//SOLO MODIFICO SECTOR INDICADO
void escribirRegistro(MV *maquina, unsigned int op, unsigned int valor){
    int reg = op & 0x1F;
    int sector = (op >> 5) & 0x03;
    unsigned int *r = &maquina->registros[reg];

    switch(sector){
        case 1:  *r = (*r & 0xFFFFFF00) | (valor & 0xFF);         break; // AL
        case 2:  *r = (*r & 0xFFFF00FF) | ((valor & 0xFF) << 8);  break; // AH
        case 3:  *r = (*r & 0xFFFF0000) | (valor & 0xFFFF);       break; // AX
        default: *r = valor;                                             // EAX
    }
}



void guardarValor(MV *maquina, unsigned int op, unsigned int valor){
    int tipo = op >> 24;

    if (tipo == 1){
        escribirRegistro(maquina, op, valor);
    }else if (tipo ==3){
        int offset = (int16_t)((op >> 8) & 0xFFFF);
        int codReg = op & 0x1F;
        int direcLogica = maquina->registros[codReg] + offset;
        accesoamemoria(maquina, direcLogica, tamanioOperando(op),1,valor);
    }

}



//dos operandos 
void MOV (MV *maquina ){
    int valor = obtenerValor(maquina, maquina->registros[OP2]);
    long long resultado64= (long long)(unsigned int) valor;
    int resultado= (unsigned int) resultado64; //LO PASO A 32 BITS

    guardarValor(maquina,maquina->registros[OP1], (unsigned int) resultado);
    actualizarCC(maquina,resultado);

    if ((long long)(unsigned int)resultado != resultado64)
        maquina->registros[CC] |= 0x20000000; //ACTIVA CARRY

    if ((long long)(int)resultado != resultado64)
        maquina->registros[CC] |= 0x10000000; //ACTIVA OVERFLOW
}


void ADD (MV *maquina ){
    int a = obtenerValor(maquina, maquina->registros[OP1]); 
    int b = obtenerValor(maquina, maquina->registros[OP2]);

    long long resultado64= (long long)(unsigned int)a + (long long)(unsigned int)b; 
    int resultado= (unsigned int) resultado64; //LO PASO A 32 BITS

    guardarValor(maquina,maquina->registros[OP1], (unsigned int) resultado);
    actualizarCC(maquina,resultado);

    if ((long long)(unsigned int)resultado != resultado64)
        maquina->registros[CC] |= 0x20000000; //ACTIVA CARRY

    if ((long long)(int)resultado != resultado64)
        maquina->registros[CC] |= 0x10000000; //ACTIVA OVERFLOW

}


void SUB(MV *maquina) {

    int valorA = obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);

    // Resta sin signo: para calcular carry
    long long resultado64 =(long long)(unsigned int)valorA -(long long)(unsigned int)valorB;

    // Resta con signo: para calcular overflow
    long long resultadoSigno =(long long)valorA - (long long)valorB;

    int resultado = (unsigned int)resultado64;

    guardarValor(maquina, maquina->registros[OP1], (unsigned int)resultado);
    actualizarCC(maquina, resultado);

    // CARRY: hubo préstamo en la resta
    if (valorA < valorB)
        maquina->registros[CC] |= 0x20000000;

    // OVERFLOW: el resultado no entra en 32 bits con signo
    if (resultadoSigno > INT32_MAX || resultadoSigno < INT32_MIN)
        maquina->registros[CC] |= 0x10000000;
}

void MUL (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);

    long long resultado64= (long long)(unsigned int)valorA * (long long)(unsigned int)valorB;
    int resultado= (unsigned int) resultado64; //LO PASO A 32 BITS

    guardarValor(maquina,maquina->registros[OP1], (unsigned int) resultado);
    actualizarCC(maquina,resultado);

    if ((long long)(unsigned int)resultado != resultado64)
        maquina->registros[CC] |= 0x20000000; //ACTIVA CARRY

    if ((long long)(int)resultado != resultado64)
        maquina->registros[CC] |= 0x10000000; //ACTIVA OVERFLOW

}

void DIV(MV *maquina){
    int valorA = obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);

    if (valorB == 0){
        printf("Error: division por cero\n");
        maquina->registros[IP] = 0xFFFFFFFF;
        return;
    }

    long long resultado64= (long long)(unsigned int)valorA / (long long)(unsigned int)valorB;
    int resultado= (unsigned int) resultado64; //LO PASO A 32 BITS
    int resto = valorA % valorB;

    guardarValor(maquina, maquina->registros[OP1], resultado);

    maquina->registros[AC] = resto; // AC

    actualizarCC(maquina, resultado);
    if ((long long)(unsigned int)resultado != resultado64)
        maquina->registros[CC] |= 0x20000000; //ACTIVA CARRY

    if ((long long)(int)resultado != resultado64)
        maquina->registros[CC] |= 0x10000000; //ACTIVA OVERFLOW
}

void CMP (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);
    long long resultado64= (long long)(unsigned int)valorA - (long long)(unsigned int)valorB;
    int resultado= (unsigned int) resultado64; //LO PASO A 32 BITS

    actualizarCC(maquina,resultado);

    if ((long long)(unsigned int)resultado != resultado64)
        maquina->registros[CC] |= 0x20000000; //ACTIVA CARRY

    if ((long long)(int)resultado != resultado64)
        maquina->registros[CC] |= 0x10000000; //ACTIVA OVERFLOW

}
void AND (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);

    int resultado = valorA & valorB;

    guardarValor(maquina, maquina->registros[OP1], resultado);

    actualizarCC(maquina, resultado);
}

void OR (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);

    int resultado = valorA | valorB;

    guardarValor(maquina, maquina->registros[OP1], resultado);

    actualizarCC(maquina, resultado);
}

void XOR (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);

    int resultado = valorA ^ valorB;

    guardarValor(maquina, maquina->registros[OP1], resultado);

    actualizarCC(maquina, resultado);
}

void SWAP (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);

    guardarValor(maquina, maquina->registros[OP1], valorB);
    guardarValor(maquina, maquina->registros[OP2], valorA);

    actualizarCC(maquina, valorB);
}
void SHL (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);

    int resultado = valorA << valorB;

    guardarValor(maquina, maquina->registros[OP1], resultado);

    actualizarCC(maquina, resultado);

}
void SHR (MV *maquina ){
    unsigned int valorA = obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);

    int resultado = valorA >> valorB;

    guardarValor(maquina, maquina->registros[OP1], resultado);

    actualizarCC(maquina, resultado);

}
void SAR (MV *maquina ){
    unsigned int valorA = (unsigned int) obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);

    unsigned int resultado = valorA;

    for (int i = 0; i < valorB; i++){

        if (resultado & 0x80000000) // si el bit de signo era 1
            resultado = (resultado >> 1) | 0x80000000;
        else
            resultado = resultado >> 1;
    }
    guardarValor(maquina, maquina->registros[OP1], resultado);

    actualizarCC(maquina, resultado);
}
void LDH (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);

    int resultado = (valorB & 0x0000FFFF) << 16 | (valorA & 0x0000FFFF);
    guardarValor(maquina, maquina->registros[OP1], resultado);
}
void LDL (MV *maquina ){
    int valorA = obtenerValor(maquina, maquina->registros[OP1]);
    int valorB = obtenerValor(maquina, maquina->registros[OP2]);

    int resultado = (valorA & 0xFFFF0000) | (valorB & 0x0000FFFF);

    guardarValor(maquina, maquina->registros[OP1], resultado);
}
void RND (MV *maquina ){
    int limite = obtenerValor(maquina, maquina->registros[OP2]);
    int resultado = rand() % (limite + 1);
    guardarValor(maquina, maquina->registros[OP1], resultado);
}

//un operando 
void imprimirBinario(unsigned int valor, int cantBits){
    printf("0b");
    for (int i = cantBits - 1; i >= 0; i--)
        printf("%d", (valor >> i) & 0x1);
    printf(" ");
}
// SYS 1 y 2  QUE ERA LO DE LA PRIMER PARTE!
void sysLeerEscribir (MV *maquina, int num ){
    unsigned int eax = maquina->registros[EAX];
    unsigned int edx = maquina->registros[EDX];
    unsigned int ecx = maquina->registros[ECX];

    int cantidad = ecx & 0xFFFF;
    int tamanio  = (ecx >> 16) & 0xFFFF;

        for (int i = 0; i < cantidad; i++){
        unsigned int dirLogica = edx + i * tamanio;
        int dirFis = direccionFisica(maquina, dirLogica, tamanio);
        

        if (dirFis == -1) {
        maquina->registros[IP] = 0xFFFFFFFF;
        return;
        }
        printf("[%04X]: ",dirFis);
        if (num==1){ //READ
            int valorLeido=0;
            if (eax & 0x02){
                char c;
                scanf("%c", &c);
                valorLeido= (unsigned char) c;   
            }
            else if (eax & 0x10){
                    char texto[64];
                    scanf("%63s", texto);
                    valorLeido = (int) strtol(texto, NULL, 2);
                }
            else if (eax & 0x08){
                scanf("%x", &valorLeido);
            }
            else if (eax & 0x04){
                scanf("%o", &valorLeido);
            }
            else {
                scanf("%d", &valorLeido);
            }

            for (int b = 0; b < tamanio; b++)
                maquina->memoria[dirFis + b] = (valorLeido >> (8*(tamanio-1-b))) & 0xFF;
        }
        else if(num == 2){ // WRITE
            unsigned int valor = 0;
            for (int b = 0; b < tamanio; b++)
                valor = (valor << 8) | maquina->memoria[dirFis + b];

            if (eax & 0x10) imprimirBinario(valor, tamanio * 8);
            if (eax & 0x08) printf("0x%X ", valor);
            if (eax & 0x04) printf("0o%o ", valor);
            if (eax & 0x02){
                unsigned char c = valor & 0xFF;
                printf("%c ", isprint(c) ? c : '.');
            }
            if (eax & 0x01) printf("%d ", (int) valor);
        }

        printf("\n");
    
            }
}
//SYS 3: lee string de teclado y guarda en [EDX] con ´\0´ al final de la cadena
void sysLeerString(MV *maquina){
    unsigned int dir = maquina->registros[EDX];
    int max = (int) maquina->registros[ECX];      // -1 = sin límite
    int leidos = 0;
    int c;

    while ((c = getchar()) != '\n' && c != EOF){
        if (max == -1 || leidos < max){
            accesoamemoria(maquina, dir, 1, 1, c);   // escribo 1 byte
            if (maquina->registros[IP] == 0xFFFFFFFF) // se salió del segmento
                return;
            dir++;
            leidos++;
        }
        // si ya llegó al máximo, sigue leyendo para descartar el resto de la línea
    }
    accesoamemoria(maquina, dir, 1, 1, 0);           // '\0' al final
}


// SYS 4: imprime desde [EDX] hasta encontrar un '\0'
void sysEscribirString(MV *maquina){
    unsigned int dir = maquina->registros[EDX];   // dirección lógica del primer carácter

    while (1){
        int c = accesoamemoria(maquina, dir, 1, 0, 0) & 0xFF;   // leo 1 byte
        if (maquina->registros[IP] == 0xFFFFFFFF)  // se salió del segmento
            return;
        if (c == 0) // es ultimo caracter->nulo                         
            break;
        printf("%c", c);
        dir++;                       
    }
}

// SYS 0: limpia la pantalla
void sysLimpiarPantalla(MV *maquina){
    system("cls");
}

void SYS (MV *maquina){
    int num = obtenerValor(maquina, maquina->registros[OP1]);

    switch (num){
        case 1:
        case 2:
            sysLeerEscribir(maquina, num);   // READ / WRITE de la Parte I
            break;
        case 3:
            sysLeerString(maquina);        break;
        case 4: 
            sysEscribirString(maquina);    break;
        case 0: 
            sysLimpiarPantalla(maquina);   break;
        case 0xF:
            break; ///FALTTTTTTTTAAAAAAA        
        default:
            printf("Error: llamada al sistema invalida\n");
            maquina->registros[IP] = 0xFFFFFFFF;
    }
}


//SE USA PARA TODOS LOS SALTOS Y EL CALL PARA MANTENER EL SEMEGNTO CODIGO Y CAMBIAR SOLO LOS 2 BYTES MAS BAJOS DEL IP 
void saltar(MV *maquina){
    unsigned int destino = obtenerValor(maquina, maquina->registros[OP1]);
    maquina->registros[IP] = (maquina->registros[IP] & 0xFFFF0000) | (destino & 0xFFFF);
}

void JMP (MV *maquina ){ 
    saltar(maquina);
} 

void JP (MV *maquina ){
    if ((maquina->registros[CC] & 0x80000000) == 0 && (maquina->registros[CC] & 0x40000000) == 0)
        saltar(maquina);
}
void JN (MV *maquina ){
    if (maquina->registros[CC] & 0x80000000)
        saltar(maquina);
}
void JZ (MV *maquina ){
    if (maquina->registros[CC] & 0x40000000)
        saltar(maquina);
}
void JC (MV *maquina ){
    if (maquina->registros[CC] & 0x20000000)
        saltar(maquina);
}
void JV (MV *maquina ){
    if (maquina->registros[CC] & 0x10000000)
        saltar(maquina);
}
void JNP (MV *maquina ){
    if ((maquina->registros[CC] & 0x80000000) || (maquina->registros[CC] & 0x40000000))
        saltar(maquina);
}
void JNN (MV *maquina ){
    if ((maquina->registros[CC] & 0x80000000) == 0)
        saltar(maquina);
}
void JNZ (MV *maquina ){
    if ((maquina->registros[CC] & 0x40000000) == 0)
        saltar(maquina);
}
void NOT (MV *maquina ){
    int valor = obtenerValor(maquina, maquina->registros[OP1]);

    int resultado = ~valor;

    guardarValor(maquina, maquina->registros[OP1], resultado);

    actualizarCC(maquina, resultado);
}

void PUSH(MV *maquina){
    maquina->registros[SP] -= 4;

    if (maquina->registros[SP] < maquina->registros[SS]){
        printf("Error: stack overflow\n");
        maquina->registros[IP] = 0xFFFFFFFF; // TERMINA LA EJECUCION a ip le pongo -1
        return;
    }

    unsigned int valor = obtenerValor(maquina, maquina->registros[OP1]);

    accesoamemoria(maquina, maquina->registros[SP], 4, 1, valor); //guardo con 4 bytes 
}

void POP(MV *maquina){
    unsigned int segPila = maquina->registros[SS] >> 16;
    unsigned int offsetSP = maquina->registros[SP] & 0xFFFF;

    if (offsetSP + 4 > maquina->TDS[segPila].tamanio){  //SI NO HAY 4 BYTES MAS PARA SACAR
        printf("Error: stack underflow\n");
        maquina->registros[IP] = 0xFFFFFFFF; //TERMINO EJECUCION
        return;
    }

    //leo los 4 del tope
    unsigned int valor=accesoamemoria(maquina,maquina->registros[SP],4,0,0);
    guardarValor(maquina,maquina->registros[OP1],valor);
    maquina-> registros[SP]+=4;

}
//sin operandos 
void STOP(MV *maquina ) {
    printf("SE EJECUTO STOP"); //cartel para chequear
    maquina->registros[IP] = 0xFFFFFFFF;
}

void CALL(MV *maquina){
    maquina->registros[SP]-=4;

    //TIENE QUE HABER ESPACIO PARA SABER A QUE INSTRUCCION VOLVER
    if (maquina->registros[SP]< maquina->registros[SS]){
        printf("Error: stack overflow\n");
        maquina->registros[IP] = 0xFFFFFFFF;
        return;   
    }
    accesoamemoria(maquina, maquina->registros[SP], 4, 1, maquina->registros[IP]);
    saltar(maquina);

}

void RET(MV *maquina){
    unsigned int segPila = maquina->registros[SS] >> 16;
    unsigned int offsetSP = maquina->registros[SP] & 0xFFFF;

    if (offsetSP + 4 > maquina->TDS[segPila].tamanio){
        printf("Error: stack underflow\n");
        maquina->registros[IP] = 0xFFFFFFFF;
        return;
    }

    maquina->registros[IP] = accesoamemoria(maquina, maquina->registros[SP], 4, 0, 0);
    maquina->registros[SP] += 4;
}