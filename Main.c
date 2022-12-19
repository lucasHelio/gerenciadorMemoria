#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include "Struct.h"



/* Função para randomizar listas */
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

void CreateRandomOrderofProcess(int *processos, size_t processosAtivos)
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
    Process *filaProcessos[MAX_PROCESSOS];
    int processosAtivos = 0;
    int time_limit = 120;
    int t = 0;
    
    // cria a memoria principal e a area de swap
    Queue *memoriaPrincipal = CreateQueue(MAX_FRAMES);
    Queue *areaDeSwap = CreateQueue(MAX_SWAP);
    

    while (1)
    {
        if(t > time_limit) break;
        
        // cria um novo processo 
        if (processosAtivos < MAX_PROCESSOS){
            filaProcessos[processosAtivos] = CreateProcess(processosAtivos);
            processosAtivos++; 
            printf("\e[0;35m+\e[0m [%03d] Processo #%d criado (%d processo(s))\n", t, processosAtivos-1, processosAtivos);
        }

        int processos[processosAtivos];
        CreateRandomOrderofProcess(processos, processosAtivos);

        for (int i = 0; i < processosAtivos; i++)
        {
            Page *pagina;
            int paginaID;
            int PID = processos[i];  

            //Enquanto o id da pagina estiver na TP gera id aleatorio
            do
                paginaID = rand() % MAX_PAGINAS_PROCESSO; // gera id de pagina aleatorio
            while (filaProcessos[PID]->tabelaPaginas[paginaID]->pagina->isInMP == 1);
            printf("\e[1;36m?\e[0m Pagina %d do processo %d requisitada e não esta na memoria principal.\n", paginaID, PID);

            ElementQueue *elementoSwap = SearchElement2(areaDeSwap, paginaID, PID);

             // Processo na área de swap
            if (elementoSwap != (ElementQueue *)NULL){ 
                printf("\e[1;33m>\e[0mPagina %d do processo %d requisitada e esta na area de swap.\n", paginaID, PID);
                RemoveElement(&areaDeSwap, elementoSwap);        // removemos do swap
            }

            pagina = CreatePage(paginaID, PID); //alocamos pagina estando ou não no swap

            ElementQueue *elemento = CreateElement(memoriaPrincipal, pagina);                                // LRU da MP
            ElementQueue *elemento2 = CreateElement(filaProcessos[PID]->paginasNaMemoriaPrincipal, pagina); // LRU do processo

            if (filaProcessos[PID]->paginasNaMemoriaPrincipal->size < WORKING_SET_LIMIT)
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

                // Atualiza TP do Processo que alocou a pagina
                InsertElementinTP(filaProcessos[PID], elemento);

            }
            else
            {
                printf("\n\n\e[1;32mv\e[0m Processo %d atingiu working set limit.\n", PID);
                
                ElementQueue *removido = Insert(&(filaProcessos[PID]->paginasNaMemoriaPrincipal), elemento2);   //  Atualiza LRU do Processo que alocou a pagina
                pagina->isInMP = 1;

                ElementQueue *elementoMP = filaProcessos[PID]->tabelaPaginas[removido->pagina->paginaID]; // Seleciona da TP a pagina a ser removida da MP
                removeFromMP(&memoriaPrincipal, elementoMP);
                
                filaProcessos[PID]->tabelaPaginas[removido->pagina->paginaID]->pagina->isInMP = 0; // Remove da TP

                Insert(&memoriaPrincipal, elemento);

                AllocPage(pagina, memoriaPrincipal);
                InsertPageinSwap(&areaDeSwap, removido);       //removido->pagina->isInMP = 0;
                InsertElementinTP(filaProcessos[PID], elemento);    // Atualiza tabela de paginas do Processo que alocou a pagina

            }
        }
        printMP(memoriaPrincipal);
        
        t += 3;
        sleep(3);
    }
    return 0;
}
