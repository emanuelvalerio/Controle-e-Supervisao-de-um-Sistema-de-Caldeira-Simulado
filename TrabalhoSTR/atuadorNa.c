 /* Monitor sensor, no arquivo sensor.c */ 
#include <math.h>
#include <pthread.h>
#include "atuadorNa.h"
#include <stdio.h>

static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; 

static double atuador_lidoNa = 0;

static double limite_atual = HUGE_VAL;
 
 /* ATUADOR Na
 /* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void atuador_put_Na( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 atuador_lidoNa = lido; 
	 pthread_mutex_unlock( &exclusao_mutua); 
 }
 
 /* Chamado por qualquer thread que precisa do valor lido do sensor */ 
 double atuador_get_Na( void) {
	 double aux; 
	 pthread_mutex_lock( &exclusao_mutua); 
	 aux = atuador_lidoNa; 
	 pthread_mutex_unlock( &exclusao_mutua); 
	 return aux; 
 }
