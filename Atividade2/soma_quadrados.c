// Francisco Losada Totaro - 10364673
// Pedro Henrique Lanfredi Moreiras - 10441998

#include "mpi.h"     // Biblioteca para programação paralela com MPI
#include <stdio.h>   // Para entrada e saída
#include <stdlib.h>    

//Para compilar: mpicc -o soma_quadrados soma_quadrados.c
//Para Executar: mpirun -np 4 ./soma_quadrados

int main(int argc, char* argv[]) {
    int id; 
  	int num_processes;       
    int n = 40;
    int i;            
  	int *vetor_global = NULL;
    int *vetor_local;
	int tam_local;
	int soma_local = 0;     
    int soma_global = 0;
  	int soma_sequencial = 0;
  
    MPI_Init(&argc, &argv); // Inicializa o ambiente MPI

    MPI_Comm_rank(MPI_COMM_WORLD, &id);            // Obtém o rank (id) do processo atual
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes); // Obtém o número total de processos

    if (id == 0) {
      	vetor_global = (int*)malloc(n * sizeof(int));
        for(int i = 0; i < n; i++){
          vetor_global[i] = i + 1; 
        }
    }
  
  	tam_local = n/num_processes;
    vetor_local = (int*)malloc(tam_local * sizeof(int));

 	// Scatter: distribui pedaços do vetor do root para todos os processos
    MPI_Scatter(vetor_global, tam_local, MPI_INT, vetor_local, tam_local, MPI_INT, 0, MPI_COMM_WORLD);
  	
  	printf("[VETOR]Processo %d recebeu:", id);
  	for(i = 0; i < tam_local; i++) {
      	printf(" %d", vetor_local[i]);
    }
    printf("\n");
  	
  	for(i = 0; i < tam_local; i++) {
    	soma_local = soma_local + vetor_local[i] * vetor_local[i];
      	
    }
  	printf("[SOMA]Processo %d: soma local dos quadrados =", id);
    printf(" %d\n\n", soma_local);
  
    // Reduz (soma) os resultados de todos os processos no processo 0
    MPI_Reduce(&soma_local, &soma_global, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Apenas o processo 0 imprime o resultado final e o erro
    if (id == 0) {
      	soma_sequencial = n*(n+1)*(2*n+1)/6;
        printf("[RESULTADO]Processo %d: soma paralela dos quadrados = %d\n", id, soma_global);
      	printf("[RESULTADO]Processo %d: soma sequencial dos quadrados = %d\n", id, soma_sequencial);
      	if (soma_global == soma_sequencial) {
          	printf("Os valores conferem!\n");
        }
      	else {
          	printf("Os valores não conferem :(\n");
        }
    }
  	free(vetor_global);
  	free(vetor_local);

    MPI_Finalize(); // Finaliza o ambiente MPI
    return 0;
}