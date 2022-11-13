#include <pthread.h>




/*************************************************************************
*	Monitor responsavel pelo acesso a tela
*
***/

static pthread_mutex_t tela = PTHREAD_MUTEX_INITIALIZER;

void aloca_tela( void) {
	sleep(1);
	pthread_mutex_lock(&tela);
	}

void libera_tela( void) {
	pthread_mutex_unlock(&tela);
	sleep(1);
	}

