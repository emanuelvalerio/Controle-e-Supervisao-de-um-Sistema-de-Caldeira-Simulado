 /* Monitor sensor, no arquivo sensor.c */ 
#include <math.h>
#include <pthread.h>
#include "sensorNo.h"
#include <stdio.h>

static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; 

static double sensor_lido_No = 0;

static double limite_atual = HUGE_VAL;
 
 /* SENSOR No
 /* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void sensor_put_No( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 sensor_lido_No = lido;  
	 pthread_mutex_unlock( &exclusao_mutua); 
 }
 
 /* Chamado por qualquer thread que precisa do valor lido do sensor */ 
 double sensor_get_No( void) {
	 double aux; 
	 pthread_mutex_lock( &exclusao_mutua); 
	 aux = sensor_lido_No; 
	 pthread_mutex_unlock( &exclusao_mutua); 
	 return aux; 
 }
 /* Thread fica bloqueada até o valor do sensor chegar em limite */ 

