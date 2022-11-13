/* Monitor sensor, no arquivo sensor.c */ 
#include <math.h>
#include <pthread.h>
#include "atuador.h"
#include <stdio.h>

static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; 

static double atuador_lidoNi = 0;
static double atuador_lidoNa = 0;
static double atuador_lidoNf = 0;
static double atuador_lidoQ = 0;

static double limite_atual = HUGE_VAL;

/* ATUADOR Ni
/* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void atuador_put_Ni( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 atuador_lido_Ni= lido; 
	 pthread_mutex_unlock( &exclusao_mutua); 
 }
 
 /* Chamado por qualquer thread que precisa do valor lido do sensor */ 
 double atuador_get_Ni( void) {
	 double aux; 
	 pthread_mutex_lock( &exclusao_mutua); 
	 aux = atuador_lido_Ni; 
	 pthread_mutex_unlock( &exclusao_mutua); 
	 return aux; 
 }
 /* ATUADOR Na
 /* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void atuador_put_Na( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 atuador_lido_Na = lido; 
	 if( atuador_lido_Na >= limite_atual) 
	 	pthread_cond_signal( &alarme); 
	 pthread_mutex_unlock( &exclusao_mutua); 
 }
 
 /* Chamado por qualquer thread que precisa do valor lido do sensor */ 
 double atuador_get_Na( void) {
	 double aux; 
	 pthread_mutex_lock( &exclusao_mutua); 
	 aux = atuador_lido_Na; 
	 pthread_mutex_unlock( &exclusao_mutua); 
	 return aux; 
 }
 
 /* SENSOR Nf
 /* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void atuador_put_Nf( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 atuador_lido_Nf = lido; 
	 if( atuador_lido_Nf >= limite_atual) 
	 	pthread_cond_signal( &alarme); 
	 pthread_mutex_unlock( &exclusao_mutua); 
 }
 
 /* Chamado por qualquer thread que precisa do valor lido do sensor */ 
 double atuador_get_Nf( void) {
	 double aux; 
	 pthread_mutex_lock( &exclusao_mutua); 
	 aux = atuador_lido_Nf; 
	 pthread_mutex_unlock( &exclusao_mutua); 
	 return aux; 
 }
 /* ATUADOR Q
 /* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void atuador_put_Q( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 atuador_lido_Q = lido; 
	 if( atuador_lido_Q >= limite_atual) 
	 	pthread_cond_signal( &alarme); 
	 pthread_mutex_unlock( &exclusao_mutua); 
 }
 
 /* Chamado por qualquer thread que precisa do valor lido do sensor */ 
 double atuador_get_Q( void) {
	 double aux; 
	 pthread_mutex_lock( &exclusao_mutua); 
	 aux = atuador_lido_Q; 
	 pthread_mutex_unlock( &exclusao_mutua); 
	 return aux; 
 }
 


