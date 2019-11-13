all:

destroy:
	gcc -std=c11 msgget_destroy.c -o destroy;
	./destroy;
	rm destroy;

sai:
	gcc -std=c11 -g Sistema_anti-incendio.c -o sai -lrt -lpthread;
	#ipcs -m;
	#xterm -geometry 170x58 ./sai;
	./sai;
	make -B destroy;
	rm sai;
    
cl:
	rm sai;
	#rm printItem;
	#rm ex;
