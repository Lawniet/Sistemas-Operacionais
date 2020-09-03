# Sistemas Operacionais
Trabalho apresentado ao Professor [João Victor de Araujo Oliveira](https://www.linkedin.com/in/jo%C3%A3o-victor-de-araujo-oliveira-7b06aaba/?originalSubdomain=br) como requisito parcial para obtenção de nota na disciplina de Sistemas Operacionais. Repositório criado para armazenar os códigos do trabalho.

# Trabalho 1: Simulador de um Sistema Detector de Incêndios Florestais
 
- Data de Entrega: 31/10/2019 alterado para 14/11/2019.
- Grupo: 
  - [Gustavo Rocha Flores](https://github.com/Dekkard)
  - [Lauany Reis da Silva](https://github.com/Lawniet)
  
  
## Especificações do Trabalho
  
- Linguagem de programação utilizada: C padrão.
- Sistema Operacional utilizado em testes: Unix/Linux.
- Sob recomendações foi utiilzado a biblioteca pthreads.h (ou pacote pthreads);
- Ao realizar as comunicações entre threads utilizamos técnicas de IPC com uso de Mutex com funções próprias da biblioteca pthreads.h, garantindo assim a exclusão mútua;

## Compilação do código fonte

- Para ofecer maior comodidade no processo de compilação elaboramos um arquivo de Makefile, o qual pode ser compilado através do comando "make -B sai";
- Opcionalmente também fornecemos um código de limpeza de fila de mensagens a fim de evitar erros gerados pelo uso da técnica de IPC. O arquivo msgget_destroy.c pode ser executado pelo comando "make -B destroy";

## Funcionamento do código  

O código é dividido em etapas, as quais podem ser enumeradas:

```markdown
- Plantio do campo e criação das threads sensores:
	A matriz que será palco dos incêndios é feita por uma struct com as informações da cor do fundo, o caracter representativo e o estatus, sendo 0 para normal, 1 para thread de centro, 2 para thread de borda, -1 para fogo e -2 para fogo reportado pelas threads. No loop de criação, as threads são criadas passado informações das coordenadas e a id da thread pela função monitoramento, responsável em escanear o campo.
- Os sensores, as threads:
	A fim de combater o fogo, temos a primeira linha de defesa, os sensores. Temos espalhados pela matriz, sendo 30x30, 100 sensores, com uma distância de 2 unidades de distância (u.d.) entre si, e 1 u.d. das bordas. Cada sensor é uma thread, a qual monitora 1 u.d. em sua volta. Se for detectado no campo estatus -1, o sensor cria uma mensagem que contém um número que contém as informações das coordenadas do fogo, a id da thread que o detectou a hora que foi feita o tal, juntas de tal modo que não ocupe muito espaço e que seja fácil de recuperação, fazendo operações de divisão e de módulo. O sensor, com a mensagem, realiza uma análise do menor caminho para a borda, com informações dos estatus das threads vizinhas, e envia a mensagem para o sensor mais perto da borda que não estaja destruida. Em por falar em borda, os sensores que são consideradas de borda fazem o escanemento normal das suas redondezas, mas as envia para a central, reponsável por apagar o fogo, assim como as mensagens que ele recebe dos outros sensores. Se houver do caso do fogo for em cima do sensor, em um último martírio, ela detecta os incêndios, envia e recebe as mensagens e as reenvia, mas depois disso é destruida, e o fogo não será mais detectado naquela área.
- Central e o incêndio:
	Em seguida, é feito um fork() no programa main, o processo filho será a central, o pai é quem taca fogo. A central é responsável em receber as mensagens enviadas pelos sensores, a qual contém o número que foi criado pelas threads, recuperadas realizando as devidas operações de recuperação das informações. Com o par de coordenadas em mão, é despachado uma função de bombeiro, que irá apagar o fogo, substituindo as informações de de status, mensagem e fundo da matriz.
	Assim, faz-se o fogo. O método de incêndio é feita de forma aleatória, além de que há uma chance do fogo ocorrer ou não. Como um dado de vinte lados(d20), é tirado um número, se for maior ou igual a 10, então um programa tira um local onde ocorrerá o fogo, de forma aleatória. Além disso, há uma chance pequena do fogo espalhar ainda mais do que em apenas uma coordenada. Lançando novamente o d20, se for maior ou igual a 15, o programa decidirá quantos quadrados em volta da coordenada a qual o fogo irá se alastrar. Foi imposta um limite no fogo, que somente irá continuar acontecendo desde que haja combustivél para o ato, começando com 100 cargas, se chegar à 0, o programa termina, mas que pode aumentar conforme o tempo, quando ocorrer fogo, é consumido uma carga, ao lastre, é consmido 3 cargas, mas se não houver fogo, é acrescentado uma carga.
 ```
 
## Implementação do sistema

- Memória Compartilhada:
	Bom, para compartilhar a matriz com os outros processos e threads, cria-se um memória compartilhada do seu registrador, onde é lida em cada thread e escrita de forma dinâmica pela main também
- Fila de mensagem:
	Cada thread é equipada com uma fila de mensagem, a qual usa pra ler suas mensagens e redireciona-las para as outras filas das outras threads. Quando o estatus da matriz é lida -1, cria-se a mensagem, usa a função de análise do menor caminho, dependendo dos estados das threads em sua volta, e envia a tal. Depois ela lê a sua própria fila, que faz o mesmo método de escolha da caminho para o reenvio. A central tem a sua prórpia fila de mensagem, acessada pelos sensores de borda, sendo o destino final das mensagens, as tais que seram retiradas as coordenadas para o bombeiro apagar o fogo, além de outras informações pertinentes.
- Fork:
	A fim de facilitar o código e de manter um certo grau de dualidade, na main é criada um processo que servirá de central para apagar o incêndio, em contra partida, a main é responsável pelo dito cujo.
- Threads:
	Como já foi dito, cada sensor é uma thread, sendo elas trabalhando de forma paralela para detectar o fogo. Cada thread é criada a partir da função monitoramento, passando como parâmetro as coordenadas da thread e sua id, que é usada para criar sua fila de mensagem. Uma thread termina seu funcionamento quando é destruida pelo fogo, quando o tal acontece em cima deste, e mesmo ao terminar, sua fila de mensagem ainda continua ativa, a fim de evitar problemas de falha de segmentação.
- Mutex:
	Para garantir a execução sem problemas de sobrescrita, é feita uma região exclusiva para as threads que querem ler e escrever nas filas de mensagem, interrompendo as outras threads nessa etapa de execução, assim somente aquela thread irá realizar o tal ato.
	
	Para maiores informações sobre o código e atualizações acesse: https://github.com/Lawniet/Sistemas-Operacionais/edit/master/Simulador%20de%20um%20Sistema%20Detector%20de%20Inc%C3%AAndios%20Florestais
