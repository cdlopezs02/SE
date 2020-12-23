/* 
 * File:   sensores.h
 * Author: Carlos David
 *
 * Created on 6 de noviembre de 2020, 12:59
 */

int tempint = 0;
int tempext = 0;
int humedad = 0;
int lum = 0;

int sumaLecturas = 0;
int numeroLecturas = 0;
int tempObjetivo = 0;
float lastPotenciometro = 0;

int ADCready=0;

void set_up_Sensores();

void read_temperatura_interior();

int get_temperatura_interior();


void read_temperatura_exterior();

int get_temperatura_exterior();


void read_humedad();

int get_humedad();


void read_intensidad_lumninosa();

int get_intensidad_luminosa();


void read_potenciometro();

int get_tempObjetivo();

float get_lastPotenciometro();


