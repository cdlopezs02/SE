/*
 * File:   display.c
 * Author: Carlos David
 *
 * Created on 6 de noviembre de 2020, 12:59
 */

#include "display.h"
#include <xc.h>
#include <stdio.h>
#include "sensores.h"
#include "actuadores.h"

void init_uart(void) { /* See chapter 12.3 PIC16F886 manual (table 12-5) */
    
    TXSTAbits.SYNC = 0; /* Asynchronous */
    TXSTAbits.BRGH = 0;
    BAUDCTLbits.BRG16 = 0;
    // SPBRGH:SPBRG = 
    SPBRGH = 0;
    SPBRG = 32; // 9600 baud rate with 20MHz Clock


    TXSTAbits.TX9 = 0; /* TX 8 data bit */
    RCSTAbits.RX9 = 0; /* RX 8 data bit */

    PIE1bits.TXIE = 0; /* Disable TX interrupt */
    PIE1bits.RCIE = 0; /* Disable RX interrupt */

    RCSTAbits.SPEN = 1; /* Serial port enable */

    TXSTAbits.TXEN = 0; /* Reset transmitter */
    TXSTAbits.TXEN = 1; /* Enable transmitter */

}

/* It is needed for printf */
void putch(char c) {
    while (!TXIF)
        continue;
    TXREG = c;
}

void mensajeBienvenida() {
    printf("\r\nBienvenidos al sistema de Control y Monitorizacion del Horno\r\n");
}

void mostrarConsigna() {
    printf("\r\n------> Consigna o Temperatura Objetivo: %dº\r\n", get_tempObjetivo());
}

void mostrarDatos() {
    printf("\r\n------> Datos:\r\n");

    printf("Temperatura interior %dº \r\n", get_temperatura_interior());

    printf("Temperatura exterior %dº \r\n", get_temperatura_exterior());

    printf("Humedad %d%% \r\n", get_humedad());

    printf("Intensidad luminosa %d lm\r\n", get_intensidad_luminosa());

    printf("Velocidad del ventilador %d%%\r\n", get_velocidad_ventilador());


}




