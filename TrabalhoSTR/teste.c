//Definição de Bibliotecas
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "socket.h"
#include "sensorTi.h"
#include "sensorH.h"
#include "sensorT.h"
#include "sensorNo.h"
#include "sensorTa.h"
#include "atuadorQ.h"
#include "atuadorNa.h"
#include "atuadorNf.h"
#include "atuadorNi.h"
#include "temperaturaRef.h"
#include "nivelRef.h"
#include "tela.h"
#include "bufduplo.h"

#define	NSEC_PER_SEC    (1000000000) 	// Numero de nanosegundos em um segundo

#define N_AMOSTRAS 1000

double tempDesejada=0.0,nivDesejada=0.0;

void thread_mostra_status (void){

	double temp,h,ta,no,ti,ni,na,nf,q,tempDesejada,nivDesejado;
	
	while(1){
		
		temp = sensor_get_T();
		h = sensor_get_H();
		ti = sensor_get_Ti();
		no = sensor_get_No();
		ta = sensor_get_Ta();
		
		ni = atuador_get_Ni();
		na = atuador_get_Na();
		nf = atuador_get_Nf();
		q = atuador_get_Q();
		
		nivDesejado = getNivel_ref();
		tempDesejada = getTemperatura_ref();
		
		
		aloca_tela();
		printf("\33[H\33[2J");		
		printf("---------------------------------------\n");
		printf("Temperatura escolhida pelo usuario--> %.4lf\n", tempDesejada);
		printf("Nivel de agua escolhido pelo usuario--> %.4lf\n", nivDesejado);
		printf("Temperatura (T)--> %.4lf\n", temp);
		printf("Temperatura do ambiente ao redor do recipiente (Ta)--> %.4lf\n", ta);
		printf("Temperatura da agua de entrada (Ti)--> %.4lf\n", ti);
		printf("Fluxo de agua de saida (No)--> %.4lf\n", no);
		printf("Altura (H)--> %.4lf\n", h);
		printf("Fluxo de água aquecida (Na)--> %.4lf\n", na);
		printf("Fluxo de água de saida (Nf)--> %.4lf\n", nf);
		printf("Fluxo de água de entrada (Ni)--> %.4lf\n", ni);
		printf("Fluxo de calor (Q)--> %.4lf\n", q);
		printf("---------------------------------------\n");
		libera_tela();
		sleep(1);
							
	}	
		
}


void thread_le_sensor (void){ //Le Sensores periodicamente a cada 10ms

       char msg_enviada[1000];
	struct timespec t;
	long periodo = 10e6; //10ms
	
	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec++;
	while(1){
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		//leitura
		//Envia mensagem via canal de comunicação para receber valores de sensores	
		sensor_put_T(msg_socket("st-0"));
		sensor_put_Ta(msg_socket("sta-0"));
		sensor_put_Ti(msg_socket("sti0"));
		sensor_put_No(msg_socket("sno0"));
		sensor_put_H(msg_socket("sh-0"));
		
		double aquecedor = atuador_get_Q();
		double flxAguaSaida = atuador_get_Nf();
		double flxAguaEntrada = atuador_get_Ni();
		double flxAguaQuente = atuador_get_Na();
		
		sprintf(msg_enviada,"aq-%lf",aquecedor);
		msg_socket(msg_enviada);
		
	        sprintf(msg_enviada,"anf%lf",flxAguaSaida);
		msg_socket(msg_enviada);
		
		sprintf(msg_enviada,"ani%lf",flxAguaEntrada);
		msg_socket(msg_enviada);
		
		sprintf(msg_enviada,"ana%lf",flxAguaQuente);
		msg_socket(msg_enviada);
		
		
		
		// Calcula inicio do proximo periodo
		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}		
	}		
}

void thread_alarme (void){
	while(1){
		sensor_alarmeT(30);
		aloca_tela();
		printf("ALARME\n");
		libera_tela();
		sleep(1);	
	}
		
}
/*void thread_alteraTeclado(void){
	double newTemp ;
	while(1){
	aloca_tela();
        printf("Digite o valor desejado da temperatura:\n");
        libera_tela();
	scanf("%lf", &newTemp);
	putTemperatura_ref(newTemp);
	
	sleep(1);
	
	}
	
}*/
///Controle
void thread_controle_temperatura (void){
	char msg_enviada[1000];
	long atraso_fim;
	struct timespec t, t_fim;
	long periodo = 50e6; //50ms
	double temp,ref_temp,niv,ref_niv;
	double erroT=0.0,no,erroH;
	
	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec++;
	while(1){
		
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		
		temp = sensor_get_T();
		ref_temp = getTemperatura_ref();
		niv = sensor_get_H();
		ref_niv = getNivel_ref();
		no = sensor_get_No();
		erroT = (temp-ref_temp)/ref_temp;
		erroH = (niv-ref_niv)/ref_niv;
		if(erroT<0.0000) erroT = -erroT;
		if(erroH<0.0000) erroH = -erroH;
		//erro = abs((temp-ref_temp));
		double Ti = sensor_get_Ti();

     if(temp>ref_temp) { //diminui temperatura
      	if(erroH > 0.1 && niv < ref_niv){ // aumenta nivel
    		atuador_put_Q(0.0);
    		if(Ti>temp && temp>80.0){
    			atuador_put_Ni(no+10.0);
    			atuador_put_Na(100.0);
    			atuador_put_Nf(0.0);
    		} else if(Ti>temp && temp<80.0) {
    			atuador_put_Ni(no+10.0);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(0.0);
    		} else if(Ti<temp && temp>80.0) {
    			atuador_put_Ni(no+100.0);
    			atuador_put_Na(100.0);
    			atuador_put_Nf(0.0);
    		} else {
    			atuador_put_Ni(no+100.0);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(0.0);
    		}	
        }
      if(erroH < 0.1 && niv < ref_niv){ // aumenta nivel
    		atuador_put_Q(0.0);
    		if(Ti>temp && temp>80.0){
    			atuador_put_Ni(no+0.5);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(0.0);
    		} else if(Ti>temp && temp<80.0) {
    			atuador_put_Ni(no+0.5);
    			atuador_put_Na(0.5);
    			atuador_put_Nf(0.0);
    		} else if(Ti<temp && temp>80.0) {
    			atuador_put_Ni(no+10.0);
    			atuador_put_Na(50.0);
    			atuador_put_Nf(0.0);
    		} else {
    			atuador_put_Ni(no+10.0);
    			atuador_put_Na(0.5);
    			atuador_put_Nf(0.0);
    		}	
        }
         if(erroH > 0.1 && niv > ref_niv){ // diminui nivel
    		atuador_put_Q(0.0);
    		if(Ti>temp && temp>80.0){
    			atuador_put_Ni(0.0);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(100.0);
    		} else if(Ti>temp && temp<80.0) {
    			atuador_put_Ni(0.0);
    			atuador_put_Na(0.0);
    			atuador_put_Nf(100.0);
    		} else if(Ti<temp && temp>80.0) {
    			atuador_put_Ni(10.0);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(100.0);
    		} else {
    			atuador_put_Ni(10.0);
    			atuador_put_Na(0.0);
    			atuador_put_Nf(100.0);
    		}	
        }
      if(erroH < 0.1 && niv > ref_niv){ // diminui nivel
    		atuador_put_Q(0.0);
    		if(Ti>temp && temp>80.0){
    			atuador_put_Ni(0.0);
    			atuador_put_Na(0.5);
    			atuador_put_Nf(10.0);
    		} else if(Ti>temp && temp<80.0) {
    			atuador_put_Ni(0.0);
    			atuador_put_Na(0.0);
    			atuador_put_Nf(10.0);
    		} else if(Ti<temp && temp>80.0) {
    			atuador_put_Ni(0.5);
    			atuador_put_Na(0.5);
    			atuador_put_Nf(10.0);
    		} else {
    			atuador_put_Ni(0.5);
    			atuador_put_Na(0.0);
    			atuador_put_Nf(10.0);
    		}	
        }
    }
    
     //hhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh
    if(temp<ref_temp) { //aumenta temperatura
      	if(erroH > 0.1 && niv < ref_niv){ // aumenta nivel
    		atuador_put_Q(1000000.0);
    		if(Ti>temp && temp>80.0){
    			atuador_put_Ni(no+100.0);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(0.0);
    		} else if(Ti>temp && temp<80.0) {
    			atuador_put_Ni(no+100.0);
    			atuador_put_Na(100.0);
    			atuador_put_Nf(0.0);
    		} else if(Ti<temp && temp>80.0) {
    			atuador_put_Ni(no+10.0);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(0.0);
    		} else {
    			atuador_put_Ni(no+10.0);
    			atuador_put_Na(100.0);
    			atuador_put_Nf(0.0);
    		}	
        }
      if(erroH < 0.1 && niv < ref_niv){ // aumenta nivel
    		atuador_put_Q(1000000.0);
    		if(Ti>temp && temp>80.0){
    			atuador_put_Ni(no+10.0);
    			atuador_put_Na(0.5);
    			atuador_put_Nf(0.0);
    		} else if(Ti>temp && temp<80.0) {
    			atuador_put_Ni(no+10.0);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(0.0);
    		} else if(Ti<temp && temp>80.0) {
    			atuador_put_Ni(no+0.5);
    			atuador_put_Na(0.5);
    			atuador_put_Nf(0.0);
    		} else {
    			atuador_put_Ni(no+0.5);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(0.0);
    		}	
        }
         if(erroH > 0.1 && niv > ref_niv){ // diminui nivel
    		atuador_put_Q(1000000.0);
    		if(Ti>temp && temp>80.0){
    			atuador_put_Ni(10.0);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(no+100.0);
    		} else if(Ti>temp && temp<80.0) {
    			atuador_put_Ni(10.0);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(no+100.0);
    		} else if(Ti<temp && temp>80.0) {
    			atuador_put_Ni(10.0);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(no+100.0);
    		} else {
    			atuador_put_Ni(10.0);
    			atuador_put_Na(10.0);
    			atuador_put_Nf(no+100.0);
    		}	
        }
      if(erroH < 0.1 && niv > ref_niv){ // diminui nivel
    		atuador_put_Q(0.0);
    		if(Ti>temp && temp>80.0){
    			atuador_put_Ni(0.5);
    			atuador_put_Na(0.5);
    			atuador_put_Nf(no+10.0);
    		} else if(Ti>temp && temp<80.0) {
    			atuador_put_Ni(0.5);
    			atuador_put_Na(0.5);
    			atuador_put_Nf(no+10.0);
    		} else if(Ti<temp && temp>80.0) {
    			atuador_put_Ni(0.5);
    			atuador_put_Na(0.5);
    			atuador_put_Nf(no+10.0);
    		} else {
    			atuador_put_Ni(0.5);
    			atuador_put_Na(0.5);
    			atuador_put_Nf(no+10.0);
    		}	
        }
    }
     //kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
        
		// Le a hora atual, coloca em t_fim
		clock_gettime(CLOCK_MONOTONIC ,&t_fim);
		
		// Calcula o tempo de resposta observado em microsegundos
		atraso_fim = 1000000*(t_fim.tv_sec - t.tv_sec)   +   (t_fim.tv_nsec - t.tv_nsec)/1000;
		
		bufduplo_insereLeitura(atraso_fim);
		
		// Calcula inicio do proximo periodo
		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}

		
	}
}
void thread_controle_nivel(void){
	char msg_enviada[1000];
	long atraso_fim;
	struct timespec t, t_fim;
	long periodo = 70e6; //70ms
	double niv=0.0, ref_niv=0.0,temp,ref_temp;
	double erroT=0.0,no=0.0,erroH;
	
	// Le a hora atual, coloca em t
	clock_gettime(CLOCK_MONOTONIC ,&t);
	t.tv_sec++;
	while(1){
		// Espera ateh inicio do proximo periodo
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &t, NULL);
		
		temp = sensor_get_T();
		ref_temp = getTemperatura_ref();
		niv = sensor_get_H();
		ref_niv = getNivel_ref();
		no = sensor_get_No();
		erroT = (temp-ref_temp)/ref_temp;
		erroH = (niv-ref_niv)/ref_niv;
		if(erroT<0.0000) erroT = -erroT;
		if(erroH<0.0000) erroH = -erroH;
		//erro = abs((temp-ref_temp));
		double Ti = sensor_get_Ti();
	if(niv>ref_niv){ // diminuir nivel
		    if(erroT > 0.1 && temp < ref_temp){ // aumenta temperatura
	    		atuador_put_Q(1000000.0);
	    		atuador_put_Nf(no+100.0);
	    		if(Ti>temp && temp>80.0){
	    			atuador_put_Ni(10.0);
	    			atuador_put_Na(10.0);
	    		} else if(Ti>temp && temp<80.0) {
	    			atuador_put_Ni(10.0);
	    			atuador_put_Na(10.0);
	    		} else if(Ti<temp && temp>80.0) {
	    			atuador_put_Ni(10.0);
	    			atuador_put_Na(10.0);
	    		} else {
	    			atuador_put_Ni(10.0);
	    			atuador_put_Na(10.0);
	    		}	
                   }
                if(erroT < 0.1 && temp < ref_temp){ // aumenta temperatura
	    		atuador_put_Q(1000.0);
	    		atuador_put_Nf(no+100.0);
	    		if(Ti>temp && temp>80.0){
	    			atuador_put_Ni(0.5);
	    			atuador_put_Na(0.5);
	    		} else if(Ti>temp && temp<80.0) {
	    			atuador_put_Ni(0.5);
	    			atuador_put_Na(0.5);
	    		} else if(Ti<temp && temp>80.0) {
	    			atuador_put_Ni(0.5);
	    			atuador_put_Na(0.5);
	    		} else {
	    			atuador_put_Ni(0.5);
	    			atuador_put_Na(0.5);
	    		}	
                   }
                 if(erroT > 0.1 && temp > ref_temp){ // diminuir temperatura
	    		atuador_put_Q(0.0);
	    		atuador_put_Nf(no+100.0);
	    		if(Ti>temp && temp>80.0){
	    			atuador_put_Ni(0.0);
	    			atuador_put_Na(10.0);
	    		} else if(Ti>temp && temp<80.0) {
	    			atuador_put_Ni(0.0);
	    			atuador_put_Na(0.0);
	    		} else if(Ti<temp && temp>80.0) {
	    			atuador_put_Ni(10.0);
	    			atuador_put_Na(10.0);
	    		} else {
	    			atuador_put_Ni(10.0);
	    			atuador_put_Na(0.0);
	    		}	
                   }
                if(erroT < 0.1 && temp > ref_temp){ // diminuir temperatura
	    		atuador_put_Q(0.0);
	    		atuador_put_Nf(no+100.0);
	    		if(Ti>temp && temp>80.0){
	    			atuador_put_Ni(0.0);
	    			atuador_put_Na(100.0);
	    		} else if(Ti>temp && temp<80.0) {
	    			atuador_put_Ni(0.0);
	    			atuador_put_Na(0.0);
	    		} else if(Ti<temp && temp>80.0) {
	    			atuador_put_Ni(0.5);
	    			atuador_put_Na(0.5);
	    		} else {
	    			atuador_put_Ni(0.5);
	    			atuador_put_Na(0.0);
	    		}	
                   }
		}
		//kkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkkk
	if(niv<ref_niv){ // aumentar nivel
		    if(erroT > 0.1 && temp < ref_temp){ // aumenta temperatura
	    		atuador_put_Q(1000000.0);
	    		atuador_put_Nf(0.0);
	    		if(Ti>temp && temp>80.0){
	    			atuador_put_Ni(no+100.0);
	    			atuador_put_Na(10.0);
	    		} else if(Ti>temp && temp<80.0) {
	    			atuador_put_Ni(no+100.0);
	    			atuador_put_Na(100.0);
	    		} else if(Ti<temp && temp>80.0) {
	    			atuador_put_Ni(no+10.0);
	    			atuador_put_Na(10.0);
	    		} else {
	    			atuador_put_Ni(no+10.0);
	    			atuador_put_Na(100.0);
	    		}	
                   }
                if(erroT < 0.1 && temp < ref_temp){ // aumenta temperatura
	    		atuador_put_Q(1000.0);
	    		atuador_put_Nf(10.0);
	    		if(Ti>temp && temp>80.0){
	    			atuador_put_Ni(no+10.0);
	    			atuador_put_Na(0.5);
	    		} else if(Ti>temp && temp<80.0) {
	    			atuador_put_Ni(no+10.0);
	    			atuador_put_Na(10.0);
	    		} else if(Ti<temp && temp>80.0) {
	    			atuador_put_Ni(no+0.5);
	    			atuador_put_Na(0.5);
	    		} else {
	    			atuador_put_Ni(no+0.5);
	    			atuador_put_Na(10.0);
	    		}	
                   }
                 if(erroT > 0.1 && temp > ref_temp){ // diminuir temperatura
	    		atuador_put_Q(0.0);
	    		atuador_put_Nf(0.0);
	    		if(Ti>temp && temp>80.0){
	    			atuador_put_Ni(no+10.0);
	    			atuador_put_Na(10.0);
	    		} else if(Ti>temp && temp<80.0) {
	    			atuador_put_Ni(no+10.0);
	    			atuador_put_Na(10.0);
	    		} else if(Ti<temp && temp>80.0) {
	    			atuador_put_Ni(no+10.0);
	    			atuador_put_Na(10.0);
	    		} else {
	    			atuador_put_Ni(no+10.0);
	    			atuador_put_Na(10.0);
	    		}	
                   }
                if(erroT<0.1 && temp > ref_temp){ // diminuir temperatura
	    		atuador_put_Q(0.0);
	    		atuador_put_Nf(10.0);
	    		if(Ti>temp && temp>80.0){
	    			atuador_put_Ni(no+0.5);
	    			atuador_put_Na(0.5);
	    		} else if(Ti>temp && temp<80.0) {
	    			atuador_put_Ni(no+0.5);
	    			atuador_put_Na(0.5);
	    		} else if(Ti<temp && temp>80.0) {
	    			atuador_put_Ni(no+0.5);
	    			atuador_put_Na(0.5);
	    		} else {
	    			atuador_put_Ni(no+0.5);
	    			atuador_put_Na(0.5);
	    		}	
                   }
		}
		//99999999999999999999999999999999999999999999999999999999999999999999999999999999999999


			
		// Le a hora atual, coloca em t_fim
		clock_gettime(CLOCK_MONOTONIC ,&t_fim);
		
		// Calcula o tempo de resposta observado em microsegundos
		atraso_fim = 1000000*(t_fim.tv_sec - t.tv_sec)   +   (t_fim.tv_nsec - t.tv_nsec)/1000;
		
		bufduplo_insereLeitura(atraso_fim);
		
		// Calcula inicio do proximo periodo
		t.tv_nsec += periodo;
		while (t.tv_nsec >= NSEC_PER_SEC) {
			t.tv_nsec -= NSEC_PER_SEC;
			t.tv_sec++;
		}
	}
	
}

void thread_grava_temp_resp(void){
	FILE* dados_f;
	dados_f = fopen("dados.txt", "w");
    if(dados_f == NULL){
        printf("Erro, nao foi possivel abrir o arquivo\n");
        exit(1);    
    }
	int amostras = 1;	
	while(amostras++<=N_AMOSTRAS/200){
		long * buf = bufduplo_esperaBufferCheio();		
		int n2 = tamBuf();
		int tam = 0;
		while(tam<n2)
			fprintf(dados_f, "%4ld\n", buf[tam++]); 
		fflush(dados_f);
		aloca_tela();
		printf("Gravando no arquivo...\n");
		
		libera_tela();
						
	}
	
	fclose(dados_f);	
}


int main( int argc, char *argv[]) {
	
	cria_socket(argv[1], atoi(argv[2]) );
    	printf("Digite o valor desejado da temperatura:\n");
	scanf("%lf", &tempDesejada);
	putTemperatura_ref(tempDesejada);
	
	printf("Digite o valor desejado do nivel maximo de agua:\n");
	scanf("%lf", &nivDesejada);
	putNivel_ref(nivDesejada);
    
	
    
	pthread_t t1, t2, t3, t4, t5,t6;
    
    pthread_create(&t1, NULL, (void *) thread_mostra_status, NULL);
    pthread_create(&t2, NULL, (void *) thread_le_sensor, NULL);
    pthread_create(&t3, NULL, (void *) thread_alarme, NULL);
    pthread_create(&t4, NULL, (void *) thread_controle_temperatura, NULL);
    pthread_create(&t5, NULL, (void *) thread_grava_temp_resp, NULL);
    pthread_create(&t6, NULL, (void *) thread_controle_nivel, NULL);
    
	pthread_join( t1, NULL);
	pthread_join( t2, NULL);
	pthread_join( t3, NULL);
	pthread_join( t4, NULL);
	pthread_join( t5, NULL);
	pthread_join( t6, NULL);
	
	return 0;
	    
}
