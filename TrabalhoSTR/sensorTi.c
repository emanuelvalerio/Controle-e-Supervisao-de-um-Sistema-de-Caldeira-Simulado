 /* Monitor sensor, no arquivo sensor.c */ 
#include <math.h>
#include <pthread.h>
#include "sensorTi.h"
#include <stdio.h>

static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; 

static double sensor_lido_Ti = 0;

static double limite_atual = HUGE_VAL;
 
 /* SENSOR Ti
 /* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void sensor_put_Ti( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 sensor_lido_Ti = lido; 
	 pthread_mutex_unlock( &exclusao_mutua); 
 }
 
 /* Chamado por qualquer thread que precisa do valor lido do sensor */ 
 double sensor_get_Ti( void) {
	 double aux; 
	 pthread_mutex_lock( &exclusao_mutua); 
	 aux = sensor_lido_Ti; 
	 pthread_mutex_unlock( &exclusao_mutua); 
	 return aux; 
 }
 

