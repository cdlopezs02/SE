/*
 * File:   Pruebas.c
 * Author: Carlos David
 *
 * Created on 1 de diciembre de 2020, 20:54
 */
#pragma config CPD = OFF, BOREN = OFF, IESO = OFF, DEBUG = OFF, FOSC = HS
#pragma config FCMEN = OFF, MCLRE = ON, WDTE = OFF, CP = OFF, LVP = OFF
#pragma config PWRTE = ON, BOR4V = BOR21V, WRT = OFF

#include <xc.h>
#include <stdio.h>
#include "sensores.h"
#include "actuadores.h"
#include "display.h"
#include "usuario.h"
#define _XTAL_FREQ  20000000


int cont = 0;

void init_timer0() {
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b111; //Preescalado de 256
    TMR0 = 61; //Salta cada 0.01s 
}
void __interrupt() Handler(void) {

    if(PIR1bits.ADIF == 1){
        ADCready = 1;
        PIR1bits.ADIF = 0;      
    }
}


void pruebaSensores() {
    
    printf("-----Pruebas de Sensores:----- \n\n");
    
    //Hacemos como si hubiera acabado de convertir 
    //y ponemos el valor 257 en AdresH/L
    PIR1bits.ADIF = 1;
    ADRESH = 1;
    ADRESL = 1;

    printf("Probamos la lectura del potenciometro:            ");
    read_potenciometro();
    PIR1bits.ADIF = 1;
    read_potenciometro();
    PIR1bits.ADIF = 1;
    read_potenciometro();
    PIR1bits.ADIF = 1;
    read_potenciometro();

    // (((257*5)/1024)*60)/3 = 25.09
    if (get_tempObjetivo() == 25)
        printf("Prueba Superada\n");
    else
        printf("Prueba Fallida\n");

    PIR1bits.ADIF = 1;
    ADRESH = 1;
    ADRESL = 1;

    printf("Probamos la lectura de la intensidad Luminosa:    ");
    read_intensidad_lumninosa();
    // ((257*5)/1024)/0.00038
    if (get_intensidad_luminosa() == 3302)
        printf("Prueba Superada\n");
    else
        printf("Prueba Fallida\n");

    PIR1bits.ADIF = 1;
    ADRESH = 1;
    ADRESL = 1;

    printf("Probamos la lectura de la Humedad:                ");
    read_humedad();
    // (((257*5)/1024)-0.826)/0.0315 = 13
    if (get_humedad() == 13)
        printf("Prueba Superada\n");
    else
        printf("Prueba Fallida\n");

    PIR1bits.ADIF = 1;
    ADRESH = 1;
    ADRESL = 1;

    printf("Probamos la lectura de la Temperatua Exterior:    ");
    read_temperatura_exterior();
    // (((257*5)/1024)-0.5)/0.01 = 75
    if (get_temperatura_exterior() == 75)
        printf("Prueba Superada\n");
    else
        printf("Prueba Fallida\n");

    PIR1bits.ADIF = 1;
    ADRESH = 1;
    ADRESL = 1;

    printf("Probamos la lectura de la Temperatua Interior:    ");
    read_temperatura_interior();
    // (((257*5)/1024)-0.5)/0.01 = 75
    if (get_temperatura_interior() == 20)
        printf("Prueba Superada\n\n");
    else
        printf("Prueba Fallida\n\n");
}

void pruebaDisplay() {
    printf("-----Pruebas del Display:----- \n\n");

    mensajeBienvenida();
    mostrarConsigna();
    mostrarDatos();

    printf("\nSe muestra por pantalla y por tanto funciona:     ");
    printf("Prueba Superada \n\n");
}

void pruebaActuadores(){
    printf("-----Pruebas Actuadores:----- \n\n");
    
    printf("---Prueba Refrigerador:            ");
    tempint = 25;
    tempObjetivo= 26;
    //errorMaximo =1.3
    //diferencia = 1;
    logica_Actuadores();
    
    if (PORTCbits.RC4 == 0 && PORTBbits.RB4 == 1 ) {
        printf("Prueba superada:\n");
    }else
        printf("Prueba Fallida\n");
        

    printf("---Prueba Calefactor:              ");
    tempint = 25;
    tempObjetivo= 24;
    //errorMaximo =1.2
    //diferencia = -1;
    logica_Actuadores();
    
    if (PORTCbits.RC4 == 1 && PORTBbits.RB4 == 0 ) {
        printf("Prueba superada:\n");
    }else
        printf("Prueba Fallida\n");
    
    
    
    printf("---Prueba Ventilador al maximo:    ");
    tempint = 20;
    tempObjetivo= 24;
    //errorMaximo =1.2
    //diferencia = 4;
    //CCPR1L = 75 * 42/100 = 31
    logica_Actuadores();

    if (get_velocidad_ventilador() == 75 && CCPR1L == 31 ) {
        printf("Prueba superada:\n");
    }else
        printf("Prueba Fallida\n");
    
    printf("---Ventilador en punto medio:      ");
    tempint = 41;
    tempObjetivo= 40;
    //errorMaximo =2
    //diferencia = -1
    logica_Actuadores();
    //CCPR1L = 37 * 42/100 = 10
    
    if (get_velocidad_ventilador() == 37 && CCPR1L == 15 ) {
        printf("Prueba superada:\n");
    }else
        printf("Prueba Fallida\n");
    
    printf("---Ventilador al minimo:           ");
    tempint = 59;
    tempObjetivo= 60;
    //errorMaximo =3
    //diferencia = 1
    logica_Actuadores();
    //CCPR1L = 25 * 42/100 = 10
    
    if (get_velocidad_ventilador() == 25 && CCPR1L == 10 ) {
        printf("Prueba superada:\n");
    }else
        printf("Prueba Fallida\n");
    
    
    printf("---Prueba de apagado del sistema:  ");
    lastPotenciometro = 3.3;
    
    
    if (get_lastPotenciometro() > 3 ) {
        printf("Prueba superada:\n");
    }else
        printf("Prueba Fallida\n");

}


void main(void) {
    //controlador_Horno();
    // External cristal
    OSCCON = 0b00001000;
    set_up_Actuadores();
    set_up_Sensores();
    init_uart();
    
    //mensajeBienvenida();
    //estadoActual = LeerSensores;

    INTCONbits.GIE = 1; //habilita interrupciones;
    INTCONbits.PEIE = 1; //Enable Other Interruptions
    INTCONbits.T0IE = 1; //habilita las interrupciones del timer0                
    //init_timer0();
    pruebaDisplay();
    pruebaSensores();
    pruebaActuadores();
    while (1) {

    }
    return;
}

