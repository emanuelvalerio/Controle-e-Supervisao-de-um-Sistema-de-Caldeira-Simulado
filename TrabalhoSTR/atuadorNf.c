 /* Monitor sensor, no arquivo sensor.c */ 
#include <math.h>
#include <pthread.h>
#include "atuadorNf.h"
#include <stdio.h>

static pthread_mutex_t exclusao_mutua = PTHREAD_MUTEX_INITIALIZER; 

static double atuador_lidoNf = 0;


static double limite_atual = HUGE_VAL;
 
 
 /* ATUADOR Nf
 /* Chamado pela thread que le o sensor e disponibiliza aqui o valor lido */
 void atuador_put_Nf( double lido) {
	 pthread_mutex_lock( &exclusao_mutua); 
	 atuador_lidoNf = lido; 
	 pthread_mutex_unlock( &exclusao_mutua); 
 }
 
 /* Chamado por qualquer thread que precisa do valor lido do sensor */ 
 double atuador_get_Nf( void) {
	 double aux; 
	 pthread_mutex_lock( &exclusao_mutua); 
	 aux = atuador_lidoNf; 
	 pthread_mutex_unlock( &exclusao_mutua); 
	 return aux; 
 }
