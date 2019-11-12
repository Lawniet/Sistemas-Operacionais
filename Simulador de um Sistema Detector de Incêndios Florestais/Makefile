all:

destroy:
	gcc -std=c11 msgget_destroy.c -o destroy;
	./destroy;
	rm destroy;

ex:
	gcc -std=c11 -g progexsh.c -o ex;
	./ex;
	rm ex;
pi:
	gcc -std=c11 -g printItem.c -o printItem;
	#xterm -geometry 97x34 ./printItem;
	./printItem;
	rm printItem;

sai:
	gcc -std=c11 -g Sistema_anti-incendio.c -o sai -lrt -lpthread;
	#ipcs -m;
	#xterm -geometry 170x58 ./sai;
	./sai;
	./destroy;
	rm sai;
    
cl:
	rm sai;
	#rm printItem;
	#rm ex;
