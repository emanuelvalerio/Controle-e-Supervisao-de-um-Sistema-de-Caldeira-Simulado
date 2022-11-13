/* Monitor sensor, no arquivo sensor.c */ 
#include <math.h>
#include <pthread.h>
#include "sensor.h"
#include <stdio.h>

static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; 

static double sensor_lidoH = 0;
static double sensor_lidoT = 0;
static double sensor_lidoTi = 0;
static double sensor_lidoNo = 0;
static double sensor_lidoTa = 0;
static double limite_atual = HUGE_VAL;

/* SENSOR H
/* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void sensor_put_H( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 sensor_lido_H= lido; 
	 if( sensor_lido_H >= limite_atual) 
	 	pthread_cond_signal( &alarme); 
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
 /* SENSOR T
 /* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void sensor_put_T( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 sensor_lido_T = lido; 
	 if( sensor_lido_T >= limite_atual) 
	 	pthread_cond_signal( &alarme); 
	 pthread_mutex_unlock( &exclusao_mutua); 
 }
 
 /* Chamado por qualquer thread que precisa do valor lido do sensor */ 
 double sensor_get_T( void) {
	 double aux; 
	 pthread_mutex_lock( &exclusao_mutua); 
	 aux = sensor_lido_T; 
	 pthread_mutex_unlock( &exclusao_mutua); 
	 return aux; 
 }
 
 /* SENSOR Ti
 /* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void sensor_put_Ti( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 sensor_lido_Ti = lido; 
	 if( sensor_lido_Ti >= limite_atual) 
	 	pthread_cond_signal( &alarme); 
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
 /* SENSOR No
 /* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void sensor_put_No( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 sensor_lido_No = lido; 
	 if( sensor_lido_No >= limite_atual) 
	 	pthread_cond_signal( &alarme); 
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
 
  /* SENSOR Ta
 /* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void sensor_put_Ta( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 sensor_lido_Ta = lido; 
	 pthread_mutex_unlock( &exclusao_mutua); 
 }
 
 /* Chamado por qualquer thread que precisa do valor lido do sensor */ 
 double sensor_get_Ta( void) {
	 double aux; 
	 pthread_mutex_lock( &exclusao_mutua); 
	 aux = sensor_lido_Ta; 
	 pthread_mutex_unlock( &exclusao_mutua); 
	 return aux; 
 }

/* Thread fica bloqueada até o valor do sensor chegar em limite */ 
void sensor_alarme( double limite) {
	pthread_mutex_lock( &exclusao_mutua); 
	limite_atual = limite; 
	limite_atual = HUGE_VAL; 
	pthread_mutex_unlock( &exclusao_mutua); 
}

