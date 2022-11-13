/* Monitor sensor, no arquivo sensor.c */ 
#include <math.h>
#include <pthread.h>
#include "sensorH.h"
#include <stdio.h>

static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; 

static double sensor_lido_H = 0;
static double limite_atual = HUGE_VAL;

/* SENSOR H
/* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void sensor_put_H( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 sensor_lido_H= lido; 
	 pthread_mutex_unlock( &exclusao_mutua); 
 }
 
 /* Chamado por qualquer thread que precisa do valor lido do sensor */ 
 double sensor_get_H( void) {
	 double aux; 
	 pthread_mutex_lock( &exclusao_mutua); 
	 aux = sensor_lido_H; 
	 pthread_mutex_unlock( &exclusao_mutua); 
	 return aux; 
 }

