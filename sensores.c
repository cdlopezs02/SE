/*
 * File:   sensores.c
 * Author: Carlos David
 *
 * Created on 6 de noviembre de 2020, 12:59
 */

#include "sensores.h"
#include <stdio.h>
#include <xc.h>



void set_up_Sensores() { //Pin de la placa     Pin del chip

    //Configuramos los pines necesarios del PortB como inputs analogicos

    //Temperatura interior ----     Pin 4               Pin 24-> B3, AN9
    ANSELHbits.ANS9 = 1; //Analogico
    TRISBbits.TRISB3 = 1; //Input

    //Temperatura exterior ----     Pin 5               Pin 23-> B2, AN8
    ANSELHbits.ANS8 = 1;
    TRISBbits.TRISB2 = 1;

    //Humedad -----------------     Pin 6               Pin 22-> B1, AN10
    ANSELHbits.ANS10 = 1;
    TRISBbits.TRISB1 = 1;

    //Intensidad luminosa -----     Pin 7               Pin 21 -> B0, AN12
    ANSELHbits.ANS12 = 1;
    TRISBbits.TRISB0 = 1;

    //Potenciometro -----------     Pin 1               Pin 26-> B5, ANS
    
    ANSELHbits.ANS13 = 1; 
    TRISBbits.TRISB5 = 1; 

    //ADC
    PIE1bits.ADIE = 1; //Enable ADC interruptions

    ADCON0bits.ADCS = 0b10; //para TAD= 1.6ms con 20Mhz de reloj
    ADCON0bits.ADON = 1; //ADC enable

    ADCON1bits.VCFG0 = 0; //referencia tierra
    ADCON1bits.VCFG1 = 0; //referencia vdd
    ADCON1bits.ADFM = 1; //formato de salida de la conversion izq0 o der1
}

void read_temperatura_interior() {
    int lectura = 0;

    ADCON0bits.CHS = 0b1001; //selecion de canal AN9

    //Hacemos pooling al flag que cambia la interrupcion y 
    //nos indica cuando ha acabado la conversion
    ADCON0bits.GO=1;
    
    while (ADCready == 0);
    ADCready = 0;
    //Hemos salido del bucle, ahora podemos leer seguros
    lectura = ADRESH << 8;
    lectura += ADRESL;
    //printf("lectura analogica: %d \r\n", lectura);
    //Pasamos la lectura a voltios y de ahí a grados
    float lecturaV = (((lectura * 5) / 1024.0)* 60)/3.75;
    tempint = (int) lecturaV;
    printf("Temperatura interior  \r\n",get_temperatura_interior());
}

int get_temperatura_interior() {

    return tempint;
}

void read_temperatura_exterior() {

    int lectura = 0;
    //Valores necesarios para el MP9700
    float V0 = 0.5; //Voltaje a 0 grados
    float Tc = 0.01; //Coeficiente de Temperatura segun internet es 100

    ADCON0bits.CHS = 0b1000; //selecion de canal AN8 conectado a pata RB2
        
    ADCON0bits.GO=1;

    //Hacemos pooling al bit ADIF de la interrupcion 
    //que nos indica cuando a acabado la conversion
    while (ADCready == 0);
    ADCready = 0;
    //Hemos salido del bucle, ahora podemos leer seguros
    lectura = ADRESH << 8;
    lectura += ADRESL;
    //Pasamos el valor obtenido de 0 a 1024 a voltios con una regla de 3
    //y despues aplicamos la formula del datasheet
    float lecturaV = (((lectura * 5) / 1024.0) - V0) / Tc;
    tempext = (int)lecturaV;
    printf("Temperatura Exterior: %d \r\n",tempext);
}

int get_temperatura_exterior() {

    return tempext;
}

void read_humedad() {
    int lectura = 0;

    ADCON0bits.CHS = 0b1010; //selecion de canal AN10 

    ADCON0bits.GO=1;
    //Hacemos pooling al flag que cambia la interrupcion y 
    //nos indica cuando ha acabado la conversion
    while (ADCready == 0);
    ADCready = 0;
    //Hemos salido del bucle, ahora podemos leer seguros
    lectura = ADRESH << 8;
    lectura += ADRESL;

    //Pasamos la lectura a voltios
    float lecturaV = (((lectura * 5) / 1024.0) -0.826)/0.0315;
    //Pasamos el valor a Humedad relativa
    humedad = (int) lecturaV;
    //Lo multiplicamos para que de un porcentaje
    //humedad = humedad * 100;
    printf("Humedad: %d \r\n",humedad);

}

int get_humedad() {
    return humedad;
}

void read_intensidad_lumninosa() {
    int lectura = 0;

    ADCON0bits.CHS = 0b1100; //selecion de canal AN12

    ADCON0bits.GO=1;
    //Hacemos pooling al flag que cambia la interrupcion y 
    //nos indica cuando ha acabado la conversion
    while (ADCready == 0);
    ADCready = 0;
    //Hemos salido del bucle, ahora podemos leer seguros
    lectura = ADRESH << 8;
    lectura += ADRESL;

        //printf("lectura analogica: %d \r\n",lectura);
    //Pasamos la lectura a voltios
    float lecturaV = ((lectura * 5) / 1024.0)/0.00038;
    //Pasamos a lumenes
    lum = lecturaV ;
    printf("Luminosidad: %d \r\n",lum);

}

int get_intensidad_luminosa() {

    return lum;
}

void read_potenciometro() {
    int lectura = 0;
    int aux = 0;
    ADCON0bits.CHS = 0b1101; //selecion de canal AN13

    ADCON0bits.GO=1;
    //Hacemos pooling al flag que cambia la interrupcion y 
    //nos indica cuando ha acabado la conversion
    while (ADCready == 0);
    ADCready = 0;
    //Hemos salido del bucle, ahora podemos leer seguros
    lectura = ADRESH << 8;
    lectura += ADRESL;

    //Pasamos la lectura a voltios
    float lecturaV = (lectura * 5) / 1024.0;
    lastPotenciometro = lecturaV;
    aux = (lecturaV * 60) / 3.0;
    sumaLecturas = sumaLecturas + aux;
    numeroLecturas++;
    
    //printf(" Lectura Analogica: %d \r\n",(int) lectura);
    printf(" Temperatura objetivo: %d \r\n",get_tempObjetivo());
}

int get_tempObjetivo() {

    if (numeroLecturas == 4) {
        tempObjetivo = sumaLecturas / numeroLecturas;
        sumaLecturas = 0;
        numeroLecturas = 0;
    }
    return tempObjetivo;
}

float get_lastPotenciometro() {
    return lastPotenciometro;
}

