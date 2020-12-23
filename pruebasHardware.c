/* 
 * File:   pruebasHardware.c
 * Author: alumno
 *
 * Created on 15 de diciembre de 2020, 14:19
 */

#pragma config CPD = OFF, BOREN = OFF, IESO = OFF, DEBUG = OFF, FOSC = HS
#pragma config FCMEN = OFF, MCLRE = ON, WDTE = OFF, CP = OFF, LVP = OFF
#pragma config PWRTE = ON, BOR4V = BOR21V, WRT = OFF

#include <xc.h>
#include <stdio.h>
#include "sensores.h"
#include "actuadores.h"
#include "display.h"
#define _XTAL_FREQ  20000000
int poten =0;
void __interrupt() Handler(void) {

    if (PIR1bits.ADIF == 1) {
        ADCready = 1;
        PIR1bits.ADIF = 0;
    }
}

void pruebaUsart() {
    mensajeBienvenida();
}

void pruebasPWM() {

    set_Ventilador(40);
}

void pruebasLecturaAnalogica() {
    int lectura = 0;
    int aux = 0;
    //ADCON0bits.CHS = 0b1101; //selecion de canal AN13
    ADCON0bits.CHS = 0b1001; //selecion de canal AN9
    ADCON0bits.CHS = 0b1001; //selecion de canal AN8

    //Hacemos pooling al flag que cambia la interrupcion y 
    //nos indica cuando ha acabado la conversion
    ADCON0bits.GO=1;
    while (ADCready == 0);
    
    ADCready = 0;
    //Hemos salido del bucle, ahora podemos leer seguros
    //printf("adresL :%d \r\n",ADRESL);
    //printf("adreslH %d\r\n", ADRESH);
    lectura = ADRESH << 8;
    lectura += ADRESL;
    
    printf("Lectura analogica: %d \r\n",lectura);
}



int main(int argc, char** argv) {
    OSCCON = 0b00001000; // External cristal
    init_uart();
    set_up_Actuadores();
    set_up_Sensores();
    INTCONbits.GIE = 1; //habilita interrupciones;
    INTCONbits.PEIE = 1; //Enable Other Interruptions
    INTCONbits.T0IE = 1; //habilita las interrupciones del timer0    
    
    while (1) {
        pruebaUsart();
        pruebasPWM();
        //pruebasLecturaAnalogica();
        set_Calefactor(1);
        set_Refrigerador(0);
        //printf("Temperatura interior: %d \r\n",get_temperatura_interior());
        read_temperatura_exterior();
        read_humedad();
        read_intensidad_lumninosa();
        read_potenciometro();
        //read_temperatura_interior();
        //printf("Temperatura Exterior: %d \r\n",get_temperatura_exterior());

        __delay_ms(5000);
    }
}





