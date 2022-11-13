
#include <math.h>
#include <pthread.h>
#include "sensorT.h"

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; 

static double temperaturaDesejada = 0.0; 

static double limite_atual = HUGE_VAL;

/* MONITOR DA TEMPERATURA DESEJADA PELO USUARIO */
/* Chamado pela thread que le o sensor e atualizar o valor do monitor referente ao sensor lido */
 void putTemperatura_ref( double lido) {
	 pthread_mutex_lock( &lock); 
	 temperaturaDesejada = lido; 
	 pthread_mutex_unlock(&lock); 
 }
 
 /* Ler o valor do monitor referente a temperatura */ 
 double getTemperatura_ref( void) {
	 double aux; 
	 pthread_mutex_lock( &lock); 
	 aux = temperaturaDesejada; 
	 pthread_mutex_unlock( &lock); 
	 return aux;
 }
