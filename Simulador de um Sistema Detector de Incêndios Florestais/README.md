# Trabalho 1: Simulador de um Sistema Detector de Incêndios Florestais
 
- Data de Entrega: 31/10/2019 alterado para 14/11/2019.
- Grupo: 
  - Gustavo Rocha Flores
  - Lauany Reis da Silva
  
  
  ## Especificações do Trabalho
  
- Linguagem de programação utilizada: C padrão.
- Sistema Operacional utilizado em testes: Unix/Linux.
- Sob recomendações foi utiilzado a biblioteca pthreads.h (ou pacote pthreads);
- Ao realizar as comunicações entre threads utilizamos técnicas de IPC com uso de Mutex com funções próprias da biblioteca pthreads.h, garantindo assim a exclusão mútua;

## Compilação do código fonte

- Para ofecer maior comodidade no processo de compilação elaboramos um arquivo de Makefile, o qual pode ser compilado através do comando "make -B sai";
- Opcionalmente também fornecemos um código de limpeza de fila de mensagens a fim de evitar erros gerados pelo uso da técnica de IPC, podendo ser executado pelo comando "make -B destroy";

## Funcionamento do código  

## Implementação do sistema
