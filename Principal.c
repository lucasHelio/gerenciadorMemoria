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


void InsertPageinSwap(Queue **areaDeSwap, ElementQueue *removido)
{
    ElementQueue *removidoSwap = Insert(areaDeSwap, removido);
    if (removidoSwap != (ElementQueue *)NULL)
    {
        printf("ÁREA DE SWAP ESTOROU!!!!\n");
        printf("CONSIDERE AUMENTAR A ÁREA SWAP!!!!\n");
    }
}

void removeFromMP(Queue **memoriaPrincipal, ElementQueue *elementoMP)
{
    if (elementoMP->anterior == (ElementQueue *)NULL)
    {
        (*memoriaPrincipal)->primeiro = elementoMP->proximo;
        elementoMP->proximo->anterior = (ElementQueue *)NULL;
    }
    else if (elementoMP->proximo == (ElementQueue *)NULL)
    {
        elementoMP->anterior->proximo = (ElementQueue *)NULL;
    }
    else
    {
        elementoMP->anterior->proximo = elementoMP->proximo;
        elementoMP->proximo->anterior = elementoMP->anterior;
    }

    (*memoriaPrincipal)->size--;
}

void printMP(Queue *memoriaPrincipal){
    ElementQueue *elemento = memoriaPrincipal->primeiro;
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
    Process *filaProcessos[NUM_PROCESSOS];
    int processosAtivos = 0;
    int time_limit = 120;
    int t = 0;
    
    // criar memoria principal
    Queue *memoriaPrincipal = CreateQueue(NUM_FRAMES);
    Queue *areaDeSwap = CreateQueue(TAM_SWAP);
    

    while (1)
    {
        if(t > time_limit) break;
        
        // cria um novo processo se necessáro
        if (processosAtivos < NUM_PROCESSOS){
            filaProcessos[processosAtivos] = CreateProcess(processosAtivos);
            processosAtivos++; // aumenta numero de processos ativos
            printf("\e[0;35m+\e[0m [%03d] Processo #%d criado (%d processo(s))\n", t, processosAtivos-1, processosAtivos);
        }

        // Cria ordem aleatória de processos dentro do vetor processos
        // de 0,1,2...,20 para qualquer ordem como 5,13,2,...,8
        int processos[processosAtivos];
        CriaOrdemAleatoriaDeProcessos(processos, processosAtivos);

        for (int i = 0; i < processosAtivos; i++)
        {
            Page *pagina;
            int paginaID;
            int PID = processos[i];  // pega o PID do vetor aleatorio de pid's

            //enquanto o id da pagina estiver na tabela de paginas gera id aleatorio
            do
                paginaID = rand() % NUM_PAGINAS_PROCESSO; // gera id de pagina aleatorio
            while (filaProcessos[PID]->tabelaPaginas[paginaID]->pagina->isInMP == 1);
            printf("\e[1;36m?\e[0m Pagina %d do processo %d requisitada e não esta na memoria principal.\n", paginaID, PID);

            ElementQueue *elementoSwap = SearchElement2(areaDeSwap, paginaID, PID);

             // encontramos na área de swap
            if (elementoSwap != (ElementQueue *)NULL){ 
                printf("\e[1;33m>\e[0mPagina %d do processo %d requisitada e esta na area de swap.\n", paginaID, PID);
                RemoveElement(&areaDeSwap, elementoSwap); // removemos do swap
            }

            pagina = CreatePage(paginaID, PID); //alocamos pagina estando ou não no swap

            ElementQueue *elemento = CreateElement(memoriaPrincipal, pagina);                                // LRU da memoria principal
            ElementQueue *elemento2 = CreateElement(filaProcessos[PID]->paginasNaMemoriaPrincipal, pagina); // LRU do processo

            if (filaProcessos[PID]->paginasNaMemoriaPrincipal->size < WORK_SET_LIMIT)
            {
                
                ElementQueue *removido = Insert(&memoriaPrincipal, elemento);
                pagina->isInMP = 1;
                AllocPage(pagina, memoriaPrincipal);
                printf("\n\e[1;31mx\e[0m Processo %d não atingiu working set limit.\n", PID);


                if (removido != (ElementQueue *)NULL)
                {
                    int processID = removido->pagina->PID;
                    RemoveElement(&(filaProcessos[processID]->paginasNaMemoriaPrincipal), removido);
                    filaProcessos[processID]->tabelaPaginas[removido->pagina->paginaID]->pagina->isInMP = 0;
                    removido->pagina->isInMP = 0;
                    InsertPageinSwap(&areaDeSwap, removido);
                }

                // Atualiza LRU do Processo que alocou a pagina
                Insert(&(filaProcessos[PID]->paginasNaMemoriaPrincipal), elemento2);

                // Atualiza tabela de paginas do Processo que alocou a pagina
                InsertElementinTP(filaProcessos[PID], elemento);

            }
            else
            {
                printf("\n\n\e[1;32mv\e[0m Processo %d atingiu working set limit.\n", PID);
                //  Atualiza LRU do Processo que alocou a pagina
                ElementQueue *removido = Insert(&(filaProcessos[PID]->paginasNaMemoriaPrincipal), elemento2);
                pagina->isInMP = 1;
                // pega ponteiro da tabela de paginas da pagina a ser removida
                ElementQueue *elementoMP = filaProcessos[PID]->tabelaPaginas[removido->pagina->paginaID];
                removeFromMP(&memoriaPrincipal, elementoMP);
                

                // remove da tabela de paginas
                filaProcessos[PID]->tabelaPaginas[removido->pagina->paginaID]->pagina->isInMP = 0;

                Insert(&memoriaPrincipal, elemento);

                AllocPage(pagina, memoriaPrincipal);
                //removido->pagina->isInMP = 0;
                InsertPageinSwap(&areaDeSwap, removido);

                // Atualiza tabela de paginas do Processo que alocou a pagina
                InsertElementinTP(filaProcessos[PID], elemento);

            }

            
        }
        printMP(memoriaPrincipal);

        
        t += 3;
        sleep(3);

    }

    return 0;
}