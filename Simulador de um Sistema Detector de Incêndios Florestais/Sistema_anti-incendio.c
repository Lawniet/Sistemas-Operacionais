#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <string.h>
/* Defines*/
#define THREADS_MAX 100
#define MSG_SIZE_TEXT 256
#define true 1
#define clear() printf("\e[2J\e[H")
/* Cores em ANSI utilizadas */
#define ANSI_COLOR_RED          "\033[31;1;41m"
#define ANSI_COLOR_GRAY         "\033[30;1;40m"
#define ANSI_COLOR_YELLOW       "\033[33;1;43m"
#define ANSI_COLOR_GREEN        "\033[32;1;42m"
#define ANSI_COLOR_RESET        "\033[0m"
extern int errno;
/* Estrutura com dados a serem usados no mapa florestal*/
typedef struct itens{
    int id, position_x, position_y, status;
    char *color, *msg;
}itens;
//itens item[30][30];
/* Estrutura com dados a serem usados no monitoramento das threads*/
typedef struct {
int id, posX, posY, status;
}thread_arg, *ptr_thread_arg;
/* Vetor com todas threads utilizadas ao longo do código*/
pthread_t threads[THREADS_MAX];
/*Variáveis para criação e uso do Mutex, semáforo binário*/
int counter;
pthread_mutex_t lock;
/* Estrutura de dados da mensagem das filas*/
typedef struct {
    long mtype;
    char mtext[MSG_SIZE_TEXT];
} msgtext;
struct msqid_ds buf;
/*Função para criação do índice da matrix
    Parâmetros de entrada: 
    m -> matrix a ser utilizda; 
    rows e cols -> qtd de linhas e colunas; 
    sizeElement -> tamanhos dos elementos usados na matrix*/
void create_index(void **m, int rows, int cols, size_t sizeElement){
    int i;  
    size_t sizeRow = (cols * sizeElement);//ajuste do tamanho da linha
    m[0] = m+rows;//???
    for(i=1; i<rows; i++){      
        m[i] = (m[i-1]+sizeRow);//inicialização da matrix
    }
}

/*Função de monitoramento do sensor
Parâmetros de entrada: 
    threadid -> info da thread;  
    posX e posY -> respectivas posições em x e y do sensor informante*/
int least_path(int posX, int posY, int st1, int st2, int st3, int st4){
	int least = 30;
	int pos = 0;
	if(posX-1 < least && st1 > 0){
		pos = 0;
	}if(posY-1 < least && st2 > 0){
		pos = 1;
	}if(30-posX < least && st3 > 0){
		pos = 2;
	}if(30-posY < least && st4 > 0){
		pos = 3;
	}
	return pos;
}
void *monitoramento(void *param){
	ptr_thread_arg targ = (ptr_thread_arg)param;
	int threadid = targ->id, posX = targ->posX, posY = targ->posY;
	
	int idthread = threadid;
    int msqid;//Ids das filas de msg.
    int idshm1,idshm2;//Ids das Mem. Compart.
    //int i,j, posT = (posX*30+posY)/3;//posT -> regulariza as posições x e y
    int pos[4] = {idthread-10,idthread-1,idthread+10,idthread+1}; //pos -> vetor dos sensores vizinhos 
    //msgtext msg;//msg -> mensagem transmitida pelas threads
	itens **item;
    /*item = (itens**)malloc(sizeof(itens*)*30);
    for(int i=0;i<30;i++){
        item[i] = (itens*)malloc(sizeof(itens)*30);
    }*/
    pthread_t* threads;
    /*Tentativa de criação da memória compartilhada*/
    if((idshm1 = shmget(0xFFF, sizeof(itens)*900, IPC_CREAT|0777)) < 0){
        printf("erro na criacao da memória 1: %d\n",idthread);
        exit(1);
    }
    /*Tentativa de criação da memória compartilhada*/
    /*if((idshm2 = shmget(0xFFE, THREADS_MAX*sizeof(pthread_t), IPC_CREAT|0777)) < 0){
        printf("erro na criacao da memória 2: %d\n",idthread);
        exit(1);
    }*/
    item = shmat(idshm1,0,0);
    //threads = shmat(idshm2,NULL,0);
    /*Tentativa de criação da fila de mensagens*/
    if((msqid = msgget(idthread, IPC_CREAT | 0600)) == -1){
        fprintf(stderr,"1: Thread %d: Erro de msgget(): %s\n",idthread, strerror(errno));
        exit(1);
    }
    int msqid_main;
    if((msqid_main = msgget(0x1234,0)) == -1 ){
        fprintf(stderr,"2: Thread %d: Erro msgget(): %s\n", idthread,  strerror(errno));
    	exit(1);
    }
    //printf("Thread de id %d(%d,%d)\n", idthread, posX, posY);
    sleep(5);
    int exit_status = 0;
    while(true){
		/*Bloqueio da região crítica*/
		//pthread_mutex_lock(&lock);
		//counter = idthread;
		//printf ("Bloqueio para a thread %d(%d,%d)\n", counter,posX,posY);
    	if(posX==1||posX==28||posY==1||posY==28){
			for(int i=posX-1;i<=posX+1;i++){
		       for(int j=posY-1;j<=posY+1;j++){
					if(item[i][j].status == -1){
						//printf("Fogo detectado por %d(%d,%d) em (%d,%d)\n", idthread, posX, posY, i, j);
						msgtext msg;
						msg.mtype = 1;
						sprintf(msg.mtext,"%d",(((i*30)+j)+idthread*1000)+1000000);
						//printf("%s\n",msg.mtext);
						//printf("Recebido: %s\n",msg.mtext);
        				pthread_mutex_lock(&lock);
        				counter = idthread;
						if(msgsnd(msqid_main,&msg,strlen(msg.mtext),IPC_NOWAIT) == -1){
							fprintf(stderr,"3: Thread %d Envio de mensagem impossivel: %s\n", idthread, strerror(errno));
							exit(1);
						}else{
							item[i][j].status = -2;
						}
						pthread_mutex_unlock(&lock);
						if(i==posX && j==posY){
							exit_status = 1;
						}
					}
				}
			}
			/*int msqido;
			if(posX==28){
				msqido = msgget(pos[0],0);
			}if(posY==28){
				msqido = msgget(pos[1],0);
			}if(posX==1){
				msqido = msgget(pos[2],0);
			}if(posY==1){
				msqido = msgget(pos[3],0);
			}*/
			int lg;
	        long type = 1;
	        int size_msg = 20;
	        msgtext msg;
        	pthread_mutex_lock(&lock);
        	counter = idthread;
	        while((lg = msgrcv(msqid,&msg,size_msg,type,IPC_NOWAIT|MSG_NOERROR)) != -1){
	        	//printf("Thread %d: Recebido: %s\n",idthread,msg.mtext);
	            if(msgsnd(msqid_main,&msg,strlen(msg.mtext),IPC_NOWAIT) == -1){
					fprintf(stderr,"4: Thread %d: Envio de mensagem impossivel: %s\n", idthread, strerror(errno));
					exit(1);
				}
	        }
	        pthread_mutex_unlock(&lock);
	        
   		}else{
		    for(int i=posX-1;i<=posX+1;i++){
		       for(int j=posY-1;j<=posY+1;j++){
					if(item[i][j].status == -1){
						//printf("Fogo detectado por %d(%d,%d) em (%d,%d)\n", idthread, posX, posY, i, j);
						msgtext msg;
						msg.mtype = 1;
						sprintf(msg.mtext,"%d",(((i*30)+j)+idthread*1000)+1000000);
						//printf("%s\n",msg.mtext);
						pthread_mutex_lock(&lock);
						counter = idthread;
						//for(int k=0;k<4;k++){
							int msqid_t;
							int k = least_path(posX,posY,item[posX-3][posY].status,item[posX][posY-3].status,item[posX+3][posY].status,item[posX][posY+3].status);
							if((msqid_t = msgget(pos[k],0)) == -1 ){
		                        fprintf(stderr,"5: Thread %d: Erro msgget(): %s\n", idthread,  strerror(errno));
	                        	exit(1);
		                    }
							if(msgsnd(msqid_t,&msg,strlen(msg.mtext),IPC_NOWAIT) == -1){
								fprintf(stderr,"5: Thread %d Envio de mensagem impossivel: %s\n", idthread, strerror(errno));
								exit(1);
							}else{ 
								item[i][j].status = -2;
							}
							/*if(msgctl(msqid_t,IPC_RMID,&buf) == -1){
								fprintf(stderr,"5: thread %d: Erro ao destruir msgctl(): %s\n", idthread, strerror(errno));
								exit(1);
							}*/
						//}
						
						pthread_mutex_unlock(&lock);
						if(i==posX && j==posY){
							exit_status = 1;
						}
					}
		        }
		    }
		    int lg;
	        long type = 1;
	        int size_msg = 20;
	        msgtext msg;
	        pthread_mutex_lock(&lock);
        	counter = idthread;
	        while((lg = msgrcv(msqid,&msg,size_msg,type,IPC_NOWAIT|MSG_NOERROR)) != -1){
	        	//printf("Thread %d: Recebido: %s\n",idthread,msg.mtext);
	            //for(int i=0;i<4;i++){
	            	int k = least_path(posX,posY,item[posX-3][posY].status,item[posX][posY-3].status,item[posX+3][posY].status,item[posX][posY+3].status);
	            	int msqid_t;
	            	if((msqid_t = msgget(pos[k],0)) == -1 ){
                        fprintf(stderr,"6: Thread %d: Erro msgget(): %s\n", idthread,  strerror(errno));
                    	exit(1);
                    }
                    if(msgsnd(msqid_t,&msg,strlen(msg.mtext),IPC_NOWAIT) == -1){
						fprintf(stderr,"6: Thread %d: Envio de mensagem impossivel: %s\n", idthread, strerror(errno));
						exit(1);
					}
                    /*if(msgctl(msqid_t,IPC_RMID,&buf) == -1){
						fprintf(stderr,"6: thread %d: Erro ao destruir msgctl(): %s\n", idthread, strerror(errno));
						exit(1);
					}*/
            	//}
	        }
	        pthread_mutex_unlock(&lock);
		}
		//sleep(1);
		/*Desbloqueio da região crítica*/
		//printf ("Desbloqueio para a thread %d(%d,%d)\n", counter,posX,posY);
		//pthread_mutex_unlock(&lock);
		if(exit_status == 1){
			printf("Thread %d foi destruida pelo fogo!\n",idthread);
			break;
		}
    }
    /*if(msgctl(msqid_main,IPC_RMID,&buf) == -1){
		fprintf(stderr,"7: thread %d: Erro ao destruir msgctl(): %s\n", idthread, strerror(errno));
		exit(1);
	}*/
    /*if(msgctl(msqid,IPC_RMID,&buf) == -1){
		fprintf(stderr,"8: thread %d: Erro ao destruir msgctl(): %s\n", idthread, strerror(errno));
		exit(1);
	}*/
	//pthread_join(idthread, NULL);
	/*if ((shmctl(idshm1, IPC_RMID, NULL)) == -1){
		perror("Erro shmctl()");
		exit(1); 
	}*/
    /*if ((shmctl(idshm2, IPC_RMID, NULL)) == -1){
    	perror("Erro shmctl()");
		exit(1);
	}*/
}
/*Função para direcionamento da thread para o monitoramento
Parâmetros de entrada: 
    param -> valor do identificador*/
/*void *return_thread(void *param){
	ptr_thread_arg targ = (ptr_thread_arg)param;
	//printf("Thread n %d, de id %d: (%d - %d)\n", pthread_self(), targ->id,targ->posX, targ->posY);
	monitoramento(targ->id,targ->posX, targ->posY);
    //pthread_exit(NULL);
}*/

int main(){
    srand(time(0));
    int n,r,c,l;
    int next_lin = 1, next_col = 1, cont_id = 1;
    int fuel = 100;
    int idshm1,idshm2;
    int pid, erro = 0;
    char field[30][30];
    itens **item;
	thread_arg arguments[THREADS_MAX];
	
	if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return 1;
    }
	
    /*Tentativa de criação da memória compartilhada*/
    if((idshm1 = shmget(0xFFF, sizeof(itens)*900, IPC_CREAT|0777)) < 0){
        printf("erro na criacao da memória 1 - principal\n");
        exit(1);
    }
    item = shmat(idshm1,0, 0);
    create_index((void*)item,30,30,sizeof(itens));
    //pthread_t* threads;
    int thread_args[THREADS_MAX];
    int msqid;
    if((msqid = msgget(0x1234, IPC_CREAT | 0600)) == -1){
	    fprintf(stderr,"Central: Erro de msgget(): %s\n", strerror(errno));
	    exit(1);
	}
    /*Tentativa de criação da memória compartilhada*/
    /*if((idshm2 = shmget(0xFFE, THREADS_MAX*sizeof(pthread_t), IPC_CREAT|0777)) < 0){
        printf("erro na criacao da memória 2 - principal\n");
        exit(1);
    }*/
    //threads = shmat(idshm2, NULL, 0);

    for(int i=0;i<30;i++){
        next_col = 1;
        for(int j=0;j<30;j++){
            if(j == next_col && i == next_lin){
                if(i==1 || j==1 || i==28 || j==28){
                    item[i][j].status = 2;
                    item[i][j].msg = "b" ;
                }
                else{
                    item[i][j].status = 1;
                    item[i][j].msg = "c" ;
                }
                field[i][j] = 'T';
                item[i][j].id = cont_id;
                item[i][j].position_x = i;
                item[i][j].position_y = j;
                item[i][j].color = ANSI_COLOR_GRAY ;

				arguments[cont_id].posX = i;
				arguments[cont_id].posY = j;
				arguments[cont_id].status = 1;
				arguments[cont_id].id = cont_id;
                /*Trecho de criação das threads*/
				erro = pthread_create(&(threads[cont_id]), NULL, monitoramento, &(arguments[cont_id]));
				if (erro != 0)
            		printf("\nErro ao criar a thread ::[%s]\n", strerror(erro));
                next_col += 3;
                cont_id++;
            }
            else{
                field[i][j] = '*';
                item[i][j].id = -1;
                item[i][j].position_x = i;
                item[i][j].position_y = j;
                item[i][j].status = 0;
                item[i][j].color = ANSI_COLOR_GREEN;
                item[i][j].msg = "a";
            }
        }
        if (i == next_lin)
            next_lin += 3;
    }
    /*for(int i=0;i<THREADS_MAX;i++){
    	printf("%d %d %d\n",threads[i], thread_args[i], arguments[i]);
    }	*/
    //system("xterm -geometry 100x40 ./printItem;");
   	if (( pid = fork()) < 0) {
		printf("erro no fork\n");
		exit(1);
	}
	if(pid == 0){
		msgtext msg;
    	int lg;
        long type = 1;
        int size_msg = 20;
    	while(true){
    		while((lg = msgrcv(msqid,&msg,size_msg,type,IPC_NOWAIT|MSG_NOERROR)) != -1){
                int num = atoi(msg.mtext)%1000000;
                int id = num/1000;
                int pos = num%1000;
                int l = pos/30, c = pos%30;
                printf("Central: fogo em (%d,%d) detectado pela thread %d.\n",l,c,id);
                field[l][c] = '*';
		        item[l][c].msg = "a";
		        item[l][c].color = ANSI_COLOR_GREEN;
		        item[l][c].status = 0;
            }
    	}
	}else{
		/*Trecho responsável pelo incêndio*/
		sleep(2);
		while(fuel>0){
		    clear();
		    //while(true){
		    r = rand()%20;
		    //printf("%d\n",r);
		    if(r >= 10){
		        //printf("\e[2J\e[H");
		        printf("%d = fogo! ",r);
		        n = rand() % 900;
		        c = n/30;
		        l = n%30;
		        printf("em (%d,%d)!",c,l);
		        field[c][l] = '@';
		        item[c][l].msg = "f" ;
		        item[c][l].color = ANSI_COLOR_RED ;
		        item[c][l].status = -1;
		        /*Trecho de alastre de incêndio*/
		        r = rand()%20;
		        fuel-=2;
		        if(r >= 15){
		            printf(" - %d = Fogo se alastrou!",r);
		            int r1 = rand() % 9;
		            for(int i=0;i<r1;i++){
		                fuel-=1;
		                r = rand() % 9;
		                field[c-(r/3)+1][l-(r%3)+1] = '@';
		                item[c-(r/3)+1][l-(r%3)+1].msg = "f" ;
		                item[c-(r/3)+1][l-(r%3)+1].color = ANSI_COLOR_RED;
		                item[c-(r/3)+1][l-(r%3)+1].status = -1;
		            }
		        }
		        printf(" - combustível restante: %d\n",fuel);
		        for(int i=0;i<30;i++){
		            if(i==0)
		                printf("%7d",i);
		            else
		                printf("%3d",i);
		        }
		        printf("\n\n");
		        /*for(int i=0;i<30;i++){
		            printf("%-5d",i);
				    for(int j=0;j<30;j++){
				        printf("%-3c",field[i][j]);
				    }
		        printf("\n");
		        }*/
		        /*Impressão visual do mapa florestal*/
		        for(int i=0;i<30;i++){
		            printf("%-5d",i);
		            for(int j=0;j<30;j++){
		                printf("%s%2d %s",item[i][j].color,item[i][j].status, ANSI_COLOR_RESET);//field[i][j]
		            }
		            printf("\n");
		        }
		        sleep(3);
		    	}else{
				    //printf("%d\n",r);
				    fuel++;
				    sleep(1);
		        }
		    }
	    }
	printf("Sem combustível...\n");
	for(int i=1;i<=100;i++){
		if((msqid=msgget(i,0))==-1){
			perror("Erro msgget()");
			exit(1);
		}
		/* recuperacao na estrutura buf dos parametros da fila */
		if(msgctl(msqid,IPC_RMID,&buf)==-1){
			perror("Erro ao destruir msgctl()");
			exit(1);
		}
	}
	for(int i=0; i<THREADS_MAX; i++){
		pthread_join(threads[i], NULL);
	}
	if ((shmctl(idshm1, IPC_RMID, NULL)) == -1){ 
		perror("Erro shmctl()");
		exit(1); 
	}
	/*if ((shmctl(idshm2, IPC_RMID, NULL)) == -1){
		perror("Erro shmctl()");
		exit(1); 
	}*/
	/* Destuição do Mutex e finalização do código*/
	pthread_mutex_destroy(&lock);
	exit(0);
	/*printf("\nPressione ENTER para sair...\n");
	getchar();*/
}
