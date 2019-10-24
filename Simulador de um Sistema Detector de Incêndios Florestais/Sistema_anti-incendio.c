#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define true 1
#define clear() printf("\e[2J\e[H")
/* Cores em ANSI utilizadas */
#define ANSI_COLOR_RED       "\033[31;1;41m"
#define ANSI_COLOR_GRAY      "\033[30;1;40m"
#define ANSI_COLOR_YELLOW     "\033[33;1;43m"
#define ANSI_COLOR_GREEN    "\033[32;1;42m"
#define ANSI_COLOR_RESET  "\033[0m"

struct itens {
	int id, position_x, position_y, status;
	char *color, *msg;
};

int main(){
    srand(time(0));
    int n,r,c,l;
    int next_lin = 1, next_col = 1, cont_id = 0;
    int fuel = 100;
    char field[30][30];
    struct itens item[30][30];

    for(int i=0;i<30;i++){
    	next_col = 1;
        for(int j=0;j<30;j++){
    		if(j == next_col && i == next_lin)
    		{
    			if(i==1 || j==1 || i==28 || j==28)
    			{
    				field[i][j] = 'T';
					item[i][j].id = cont_id;
		        	item[i][j].position_x = i;
		        	item[i][j].position_y = j;
		        	item[i][j].status = 2;
		        	item[i][j].color = ANSI_COLOR_DARK_GRAY ;
		        	item[i][j].msg = "b" ;
    			}
    			else
    			{
    				field[i][j] = 'T';
    				item[i][j].id = cont_id;
		        	item[i][j].position_x = i;
		        	item[i][j].position_y = j;
		        	item[i][j].status = 1;
		        	item[i][j].color = ANSI_COLOR_GRAY ;
		        	item[i][j].msg = "c" ;
    			}
    			next_col += 3;
    			cont_id++;
    		}
    		else
    		{
    			field[i][j] = '*';
	        	item[i][j].id = -1;
	        	item[i][j].position_x = i;
	        	item[i][j].position_y = j;
	        	item[i][j].status = 0;
	        	item[i][j].color = ANSI_COLOR_GREEN;
	        	item[i][j].msg = "a" ;
    		}
        }
    	if (i == next_lin)
		next_lin += 3;
    }
   while(fuel>0){
    //while(true){
        r = rand()%20;
        //printf("%d\n",r);
        if(r >= 10){
          clear();
            printf("\e[2J\e[H");
            printf("%d = fogo! ",r);
            n = rand() % 900;
            c = n/30;
            l = n%30;
            printf("em (%d,%d)!",c,l);
            field[c][l] = '@';
            item[c][l].msg = "f" ;
            r = rand()%20;
            fuel-=2;
            if(r >= 10){
                printf(" - %d = Fogo se alastrou!",r);
                int r1 = rand() % 8;
                for(int i=0;i<r1;i++){
                    fuel-=1;
                    r = rand() % 9;
                    field[c-(r/3)-1][l-(r%3)-1] = '@';
                    item[c-(r/3)-1][l-(r%3)-1].msg = "fa" ;
                }    
            }
            printf(" - combustível restante: %d\n",fuel);

            for(int i=0;i<30;i++)
            {
	            if(i==0)
	            	printf("   %3d",i);
	            else
	        		printf("%3d",i);
            }
            printf("\n\n");
            for(int i=0;i<30;i++){
                printf("%-5d",i);
                for(int j=0;j<30;j++){
                    printf("%-3c",field[i][j]);            
                }
                printf("\n");
            } 
            for(int i=0;i<30;i++){
                printf("%-5d",i);
                for(int j=0;j<30;j++){
                    printf("%-3d",item[i][j].id);            
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
}
