#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(){
	struct msqid_ds buf;
	int msqid;
	/* recuperacao do ID da fila de mensagens associada a chave 123 */
	if((msqid=msgget(0x1234,0))==-1){
		perror("Erro msgget()");
		exit(1);
	}
	/* recuperacao na estrutura buf dos parametros da fila */
	if(msgctl(msqid,IPC_RMID,&buf)==-1){
		perror("Erro ao destruir msgctl()");
		exit(1);
	}
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
	printf("fila de mensagem destruída com sucesso\n");
	exit(0);
}
