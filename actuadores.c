/*
 * File:   actuadores.c
 * Author: Carlos David
 *
 * Created on 6 de noviembre de 2020, 12:59
 */

#include "actuadores.h"
#include "display.h"
#include "sensores.h"
#include <xc.h>
#include <stdio.h>
#include <pic16f886.h>

    float vent =0;
    int tempInt = 0;
    int tempObj = 0;
    int diferencia = 0;
    float errorMaximo = 0;

void init_PWM_CCP1() {
    CCP1CONbits.P1M = 0; //Single output P1A modulated rest as port pins
    CCP1CONbits.CCP1M0 = 0;
    CCP1CONbits.CCP1M1 = 0;
    CCP1CONbits.CCP1M2 = 1;
    CCP1CONbits.CCP1M3 = 1;

    //Pruebas Random
    // TRISA = 0;
    //TRISB = 0;
}

void init_timer2() {
    T2CONbits.TMR2ON = 1; //Activamos el timer 2
    T2CONbits.T2CKPS = 0b01; //Preescaldo 1:4
    T2CONbits.TOUTPS = 0b0000; //Sin postescalado
    PR2 = 42; //Numero hasta el que comparar, el resultado era 41,66666
}

void set_up_Actuadores() {
    //Refrigerador ------------     Pin 8               Pin 15 -> RC4, SDI, SDA
    TRISCbits.TRISC4 = 0; //Output

    //Calefactor --------------     Pin 3               Pin 25-> B4, ANS11
    ANSELHbits.ANS11 = 0; //Digital
    TRISBbits.TRISB4 = 0; //Output

    //Ventilador PWM -----------     Pin 2               Pin 13-> C2, P1A, CCP1
    TRISC = 0; //PuertoC output
    CCPR1L = 0; //Iniciamos el ciclo de trabajo a cero
    init_PWM_CCP1();
    init_timer2(); //Timer 2 con senial PWM de 30KHz
    //No estoy seguro de si hace falta o no poner la interrupcion o no del Timer2
    //PIE1bits.TMR2IE = 1;//habilita las interrupciones del timer2
}

void set_Refrigerador(int on) {
    //Preguntar si PORTB es el que hace el output digital
    if (on)
        PORTCbits.RC4 = 1;
    else
        PORTCbits.RC4 = 0;
}

void set_Calefactor(int on) {
    if (on)
        PORTBbits.RB4 = 1;
    else
        PORTBbits.RB4 = 0;
}

void set_Ventilador(int intensidad) {
    //Intensidad tiene que ser de 0 a 100, porcentaje   
    //Regla de 3, siendo PR2 el 100/100
    CCPR1L = intensidad * PR2 / 100;
}

void logica_Actuadores() {
    tempInt = get_temperatura_interior();
    tempObj = get_tempObjetivo();
    diferencia = tempObj - tempInt;
    errorMaximo = tempObj * 0.05;

    if (get_lastPotenciometro() > 3) {
        printf("Apagando Horno...");
        SLEEP(); //No se muy bien como va esto pero es la unica manera que he encontrado de apagarlo
    } else {
        //Apagamos o encendemos el Calefactor y Refrigerador
        if (diferencia > 0) {
            set_Refrigerador(0);
            set_Calefactor(1);
        } else {
            set_Refrigerador(1);
            set_Calefactor(0);
        }
        if(diferencia < 0 )
                diferencia = -diferencia;
        //Aumentamos o disminuimos el ventilador
        if (diferencia > errorMaximo) {
            vent = 75;
            set_Ventilador(vent);
        } else {
            vent = (75 * diferencia) / errorMaximo;
            if (vent <= 25)
                vent = 25;
            set_Ventilador(vent);
        }
    }
}

int get_velocidad_ventilador(){
    return vent;
}

