#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#include "Estruturas.h"
/* Algoritmo usado para randomizar a ordem de alocacao dos processos */
/* O Algoritmo mistura um vetor de inteiros de forma randomica */
void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void CriaOrdemAleatoriaDeProcessos(int *processos, size_t processosAtivos)
{
    for (int i = 0; i < processosAtivos; i++)
        processos[i] = i;

    shuffle(processos, processosAtivos);
}


void inserePaginaNaAreaDeSwap(Fila **areaDeSwap, FilaElemento *removido)
{
    FilaElemento *removidoSwap = Insere(areaDeSwap, removido);
    if (removidoSwap != (FilaElemento *)NULL)
    {
        printf("ÁREA DE SWAP ESTOROU!!!!\n");
        printf("CONSIDERE AUMENTAR A ÁREA SWAP!!!!\n");
    }
}

void removeDaMemoriaPrincipal(Fila **memoriaPrincipal, FilaElemento *elementoMP)
{
    if (elementoMP->anterior == (FilaElemento *)NULL)
    {
        (*memoriaPrincipal)->primeiro = elementoMP->proximo;
        elementoMP->proximo->anterior = (FilaElemento *)NULL;
    }
    else if (elementoMP->proximo == (FilaElemento *)NULL)
    {
        elementoMP->anterior->proximo = (FilaElemento *)NULL;
    }
    else
    {
        elementoMP->anterior->proximo = elementoMP->proximo;
        elementoMP->proximo->anterior = elementoMP->anterior;
    }

    (*memoriaPrincipal)->size--;
}

void printMP(Fila *memoriaPrincipal){
    FilaElemento *elemento = memoriaPrincipal->primeiro;
    printf("\n\e[0;32mEspaços alocados da MP\e[0m\n");
    
    for (int i=0; i<memoriaPrincipal->size; i++){
        printf("| Frame: %d - Proc: %d Pag: %d |\n", elemento->pagina->frameIndex, elemento->pagina->PID,elemento->pagina->paginaID);
        if(elemento->proximo != NULL){
            elemento = elemento->proximo;
        }     
    }
    printf("\n");
}

int main()
{
    srand(time(NULL));
    Processo *filaProcessos[NUM_PROCESSOS];
    int processosAtivos = 0;
    int time_limit = 120;
    int t = 0;
    
    // criar memoria principal
    Fila *memoriaPrincipal = CriaFila(NUM_FRAMES);
    Fila *areaDeSwap = CriaFila(TAM_SWAP);
    

    while (1)
    {
        if(t > time_limit) break;
        
        // cria um novo processo se necessáro
        if (processosAtivos < NUM_PROCESSOS){
            filaProcessos[processosAtivos] = CriaProcesso(processosAtivos);
            processosAtivos++; // aumenta numero de processos ativos
            printf("\e[0;35m+\e[0m [%03d] Processo #%d criado (%d processo(s))\n", t, processosAtivos-1, processosAtivos);
        }

        // Cria ordem aleatória de processos dentro do vetor processos
        // de 0,1,2...,20 para qualquer ordem como 5,13,2,...,8
        int processos[processosAtivos];
        CriaOrdemAleatoriaDeProcessos(processos, processosAtivos);

        for (int i = 0; i < processosAtivos; i++)
        {
            Pagina *pagina;
            int paginaID;
            int PID = processos[i];  // pega o PID do vetor aleatorio de pid's

            //enquanto o id da pagina estiver na tabela de paginas gera id aleatorio
            do
                paginaID = rand() % NUM_PAGINAS_PROCESSO; // gera id de pagina aleatorio
            while (filaProcessos[PID]->tabelaPaginas[paginaID]->pagina->isInMP == 1);
            printf("\e[1;36m?\e[0m Pagina %d do processo %d requisitada e não esta na memoria principal.\n", paginaID, PID);

            FilaElemento *elementoSwap = BuscaElemento2(areaDeSwap, paginaID, PID);

             // encontramos na área de swap
            if (elementoSwap != (FilaElemento *)NULL){ 
                printf("\e[1;33m>\e[0mPagina %d do processo %d requisitada e esta na area de swap.\n", paginaID, PID);
                RemoveElemento(&areaDeSwap, elementoSwap); // removemos do swap
            }

            pagina = CriaPagina(paginaID, PID); //alocamos pagina estando ou não no swap

            FilaElemento *elemento = CriaElemento(memoriaPrincipal, pagina);                                // LRU da memoria principal
            FilaElemento *elemento2 = CriaElemento(filaProcessos[PID]->paginasNaMemoriaPrincipal, pagina); // LRU do processo

            if (filaProcessos[PID]->paginasNaMemoriaPrincipal->size < WORK_SET_LIMIT)
            {
                
                FilaElemento *removido = Insere(&memoriaPrincipal, elemento);
                pagina->isInMP = 1;
                AlocaPagina(pagina, memoriaPrincipal);
                printf("\n\e[1;31mx\e[0m Processo %d não atingiu working set limit.\n", PID);


                if (removido != (FilaElemento *)NULL)
                {
                    int processID = removido->pagina->PID;
                    RemoveElemento(&(filaProcessos[processID]->paginasNaMemoriaPrincipal), removido);
                    filaProcessos[processID]->tabelaPaginas[removido->pagina->paginaID]->pagina->isInMP = 0;
                    removido->pagina->isInMP = 0;
                    inserePaginaNaAreaDeSwap(&areaDeSwap, removido);
                }

                // Atualiza LRU do Processo que alocou a pagina
                Insere(&(filaProcessos[PID]->paginasNaMemoriaPrincipal), elemento2);

                // Atualiza tabela de paginas do Processo que alocou a pagina
                InsereElementoNaTabelaDePaginas(filaProcessos[PID], elemento);

            }
            else
            {
                printf("\n\n\e[1;32mv\e[0m Processo %d atingiu working set limit.\n", PID);
                //  Atualiza LRU do Processo que alocou a pagina
                FilaElemento *removido = Insere(&(filaProcessos[PID]->paginasNaMemoriaPrincipal), elemento2);
                pagina->isInMP = 1;
                // pega ponteiro da tabela de paginas da pagina a ser removida
                FilaElemento *elementoMP = filaProcessos[PID]->tabelaPaginas[removido->pagina->paginaID];
                removeDaMemoriaPrincipal(&memoriaPrincipal, elementoMP);
                

                // remove da tabela de paginas
                filaProcessos[PID]->tabelaPaginas[removido->pagina->paginaID]->pagina->isInMP = 0;

                Insere(&memoriaPrincipal, elemento);

                AlocaPagina(pagina, memoriaPrincipal);
                //removido->pagina->isInMP = 0;
                inserePaginaNaAreaDeSwap(&areaDeSwap, removido);

                // Atualiza tabela de paginas do Processo que alocou a pagina
                InsereElementoNaTabelaDePaginas(filaProcessos[PID], elemento);

            }

            
        }
        printMP(memoriaPrincipal);

        
        t += 3;
        sleep(3);

    }

    return 0;
}