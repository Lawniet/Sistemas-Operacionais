#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define true 1
#define clear() printf("\e[2J\e[H")
int main(){
    srand(time(0));
    int n,r,c,l;
    int next_lin = 1, next_col = 1;
    int fuel = 100;
    char field[30][30];
    for(int i=0;i<30;i++){
        for(int j=0;j<30;j++){
        	field[i][j] = '*';  	
        }
    }
    for(int i=0;i<30;i++){
    	next_col = 1;
        for(int j=0;j<30;j++){
    		if(j == next_col && i == next_lin)
    		{
    			if(i==1 || j==1 || i==28 || j==28)
    				field[i][j] = 't';
    			else
    				field[i][j] = 'T';
    			next_col += 3;
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
            r = rand()%20;
            fuel-=2;
            if(r >= 10){
                printf(" - %d = Fogo se alastrou!",r);
                int r1 = rand() % 8;
                for(int i=0;i<r1;i++){
                    fuel-=1;
                    r = rand() % 9;
                    field[c-(r/3)-1][l-(r%3)-1] = '@';
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
            sleep(3);
        }else{
            //printf("%d\n",r);
            fuel++;
            sleep(1);        
        }
    }
    printf("Sem combustível...\n");
}
