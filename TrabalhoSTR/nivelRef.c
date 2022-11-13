 /* Monitor sensor, no arquivo sensor.c */ 
#include <math.h>
#include <pthread.h>
#include "sensorH.h"

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

static double nivelAgua = 0; 

static double limite_atual = HUGE_VAL;
 
 
 /* MONITOR DO NIVEL DE AGUA DESEJADO PELO USUARIO */
/* Chamado pela thread que le o sensor e atualizar o valor do monitor referente ao sensor lido */
 void putNivel_ref( double lido) {
	 pthread_mutex_lock( &lock); 
	 nivelAgua = lido;  
	 pthread_mutex_unlock(&lock); 
 }
 
 /* Ler o valor do monitor referente ao nivel de agua */ 
 double getNivel_ref( void) {
	 double aux; 
	 pthread_mutex_lock( &lock); 
	 aux = nivelAgua; 
	 pthread_mutex_unlock( &lock); 
	 return aux;
 }
