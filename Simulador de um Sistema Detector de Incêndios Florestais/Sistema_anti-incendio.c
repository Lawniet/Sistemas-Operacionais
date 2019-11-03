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
#define ANSI_COLOR_RED       "\033[31;1;41m"
#define ANSI_COLOR_GRAY      "\033[30;1;40m"
#define ANSI_COLOR_YELLOW     "\033[33;1;43m"
#define ANSI_COLOR_GREEN    "\033[32;1;42m"
#define ANSI_COLOR_RESET  "\033[0m"
/* Estrutura com dados a serem usados no mapa florestal*/
typedef struct itens{
    int id, position_x, position_y, status;
    char *color, *msg;
}itens;
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
void monitoramento(void* threadid, int posX, int posY){
    int msqid,msqido;//Ids das filas de msg.
    int idshm1,idshm2;//Ids das Mem. Compart.
    int i,j, posT = posX*30+posY;//posT -> regulariza as posições x e y
    int pos[4] = {posT-10,posT-1,posT+10,posT+1}; //pos -> vetor dos sensores vizinhos 
    struct msgtext {
        long mtype;
        char mtext[MSG_SIZE_TEXT];
    } msg;//msg -> mensagem transmitida pelas threads
    itens **item;
    pthread_t* threads;
    /*Tentativa de criação da memória compartilhada*/
    if((idshm1 = shmget(0xFFF, sizeof(itens)*900, IPC_CREAT|0x1ff)) < 0){
        printf("erro na criacao da fila\n");
        exit(1);
    }
    /*Tentativa de criação da memória compartilhada*/
    if((idshm2 = shmget(0xFFE, sizeof(pthread_t), IPC_CREAT|0x1ff)) < 0){
        printf("erro na criacao da fila\n");
        exit(1);
    }
    item = shmat(idshm1,(itens*)0,0);
    threads = shmat(idshm2,NULL,0);
    /*Tentativa de criação da fila de mensagens*/
    if((msqid = msgget(threadid, IPC_CREAT | 0600)) == -1){
        perror("Erro de msgget");
        exit(1);
    }
    while(true){
        for(i=posX-1;i<3;i++){
           for(j=posY-1;j<3;j++){
               if(item[i][j].status == -1){
                    sprintf(msg.mtext,"%d",i*30+j);
                    if(msgsnd(msqid,&msg,strlen(msg.mtext),IPC_NOWAIT) == -1){
                        perror("Envio de mensagem impossivel");
                    }
                    item[i][j].status == -2;
                }
            }
        }
        for(i=0;i<3;i++){
            if((msqido = msgget(threads[pos[i]],0)) == -1 ){
                perror("Erro msgget()");
            }
            int lg;
            long type = 1;
            int size_msg = 19;
            while((lg = msgrcv(msqido,&msg,size_msg,type,IPC_NOWAIT|MSG_NOERROR)) != -1){
                for(j=0;j<3;j++){
                    if(i != j){
                        if((msqido = msgget(threads[pos[i]],0)) == -1 ){
                            perror("Erro msgget()");
                        }
                        if(msgsnd(msqido,&msg,strlen(msg.mtext),IPC_NOWAIT) == -1){
                            perror("Envio de mensagem impossivel");
                        }   
                    }
                }
            }
        }
    }
}
/*Função para criação de id da thread
Parâmetros de entrada: 
    param -> valor do identificador*/
void *return_thread(void *param){
    int id = *((int *)(param));
    printf("id da thread %d\n", id);
    pthread_exit(NULL);
}

int main(){
    srand(time(0));
    int n,r,c,l;
    int next_lin = 1, next_col = 1, cont_id = 0;
    int fuel = 100;
    int idshm1,idshm2;
    char field[30][30];
    itens **item;
    /*Tentativa de criação da memória compartilhada*/
    if((idshm1 = shmget(0xFFF, sizeof(itens)*900, IPC_CREAT|0x1ff)) < 0){
        printf("erro na criacao da fila\n");
        exit(1);
    }
    item = shmat(idshm1,(itens*)0, 0);
    create_index((void*)item,30,30,sizeof(itens));

    pthread_t* threads;
    int thread_args[THREADS_MAX];
    /*Tentativa de criação da memória compartilhada*/
    if((idshm2 = shmget(0xFFE, THREADS_MAX*sizeof(pthread_t), IPC_CREAT|0x1ff)) < 0){
        printf("erro na criacao da fila\n");
        exit(1);
    }
    threads = shmat(idshm2, NULL, 0);

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
                /*Trecho de criação das threads*/
                /*thread_args[i] = cont_id;
                pthread_create(&threads[i], NULL, (void*)monitoramento, (void *) &thread_args[i]);//função anterior: return_thread, (void *) &thread_args[i]
                pthread_join(threads[i], NULL);*/
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
    sleep(2);
    /*Trecho responsável pelo incêndio*/
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
                    printf("%s%2c %s",item[i][j].color,field[i][j], ANSI_COLOR_RESET);
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
    printf("Sem combustível...\n");
    printf("\nPressione ENTER para sair...\n");
    getchar();
}