#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator: High-speed crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include "sensores.h"
#include "actuadores.h"
#include "display.h"

#define _XTAL_FREQ  20000000

enum estado {
    LeerPotenciometro, Sensores_Actuadores, MostrarDatos, Esperar, Apagar
};

void init_timer0() {
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.PSA = 0;
    OPTION_REGbits.PS = 0b111; //Preescalado de 256
    TMR0 = 61; //Salta cada 0.01s 
}


//Variables Globales
int cont = 0;
float potenciometro = 0;

enum estado estadoActual = LeerPotenciometro;
enum estado estadoInterrupcion = LeerPotenciometro;

void __interrupt() Handler(void) {

    if (INTCONbits.T0IF) {
        TMR0 = 61; //10ms

        if (cont == 501) //Reseteamos el contador sin saltarnos estados
            cont = 1;

        if (cont % 25 == 0 && cont != 0) { //Interrupcion cada 0.25 s
            ADCready = 1;
            INTCONbits.T0IF = 0;               
            INTCONbits.T0IE = 0;               
            estadoActual = LeerPotenciometro;
            if (cont == 100 || cont == 200 || cont == 300 || cont == 400 || cont == 500) { //Comprobamos a que estados adicionales tenemos que ir
                estadoInterrupcion = Sensores_Actuadores;
                if (cont == 500)
                    estadoInterrupcion = MostrarDatos;
            }
        } else {
            cont++;
            INTCONbits.T0IF = 0;
                           
        }

    } else if (PIR1bits.ADIF == 1) {
        ADCready = 1;
        PIR1bits.ADIF = 0;

    }
}

void main(void) {
    // External cristal
    OSCCON = 0b00001000;
    set_up_Actuadores();
    set_up_Sensores();
    init_uart();
    printf("---Set UP---\n");
    mensajeBienvenida();
    estadoActual = Esperar;

    printf("Delay inicial de 3 segunds\n");
    __delay_ms(3000);

    INTCONbits.GIE = 1; //habilita interrupciones;
    INTCONbits.PEIE = 1; //Enable Other Interruptions
    INTCONbits.T0IE = 1; //habilita las interrupciones del timer0                
    init_timer0();


    while (estadoActual != Apagar) {
        switch (estadoActual) {
            case Esperar:
                NOP();
                break;
            case LeerPotenciometro:
                printf("---Potenciometro---\n");
                //Lee y guarda las lecturas
                read_potenciometro();
                if (estadoInterrupcion == Sensores_Actuadores || estadoInterrupcion == MostrarDatos) {
                    estadoActual = Sensores_Actuadores;
                } else {
                    estadoActual = Esperar;
                    estadoInterrupcion = Esperar;
                    //Aumentas contador y rehabilitas el Timer
                    cont++;
                    INTCONbits.T0IF = 0;
                    INTCONbits.T0IE = 1;  
                }

                break;

            case Sensores_Actuadores:
                //Muestra el valor de la consigna y lo actualiza con las ultimas lecturas
                mostrarConsigna();
                //Lees de todos los sensores
                ADCready = 1;
                read_humedad();
                ADCready = 1;
                read_intensidad_lumninosa();
                ADCready = 1;
                read_temperatura_exterior();
                ADCready = 1;
                read_temperatura_interior();
                ADCready = 1;
                //Actualiza los actuadores con los ultimos valores leidos    
                logica_Actuadores();

                if (estadoInterrupcion == MostrarDatos) {
                    estadoActual = MostrarDatos;
                } else {
                    estadoActual = Esperar;
                    estadoInterrupcion = Esperar;
                    //Aumentas contador y rehabilitas el Timer
                    cont++;
                    INTCONbits.T0IF = 0;
                    INTCONbits.T0IE = 1;  
                }
                break;

            case MostrarDatos:
                //Muestra los Datos
                mostrarDatos();
                estadoActual = Esperar;
                estadoInterrupcion = Esperar;
                //Aumentas contador y rehabilitas el Timer
                cont++;
                INTCONbits.T0IF = 0;
                INTCONbits.T0IE = 1;  
                break;

            case Apagar: //Estado interrupcion 0.25s
                printf("---Estado Apagando el Horno...---\n");

                break;

        }
    }

}
