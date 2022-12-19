#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Struct.h"

int numProcessos = 0;

Process *CreateProcess(int PID)
{   
    printf("cria");
    Process *processo = (Process *)malloc(sizeof(Process));

    processo->PID = numProcessos++;
    for (int i = 0; i < MAX_PAGINAS_PROCESSO; i++)
    {
        processo->tabelaPaginas[i] = (ElementQueue *)malloc(sizeof(ElementQueue)); // não está na memória principal
        processo->tabelaPaginas[i]->pagina = (Page *)malloc(sizeof(Page));
        processo->tabelaPaginas[i]->pagina->isInMP = 0;
    }
    processo->paginasNaMemoriaPrincipal = CreateQueue(WORKING_SET_LIMIT);
    return processo;
}

void InsertElementinTP(Process* processo, ElementQueue* elemento){
    processo->tabelaPaginas[elemento->pagina->paginaID] = elemento;
}

void AllocPage(Page* pagina, Queue* memoriaPrincipal){
    int alocacoes[MAX_FRAMES];
    memset(alocacoes, 0, sizeof(alocacoes));

    // Define a fila dos frames usados
    ElementQueue *p = memoriaPrincipal->primeiro;
    while (p != NULL)
    {
        if (p->pagina->frameIndex != -1)
            alocacoes[p->pagina->frameIndex] = 1;
        p = p->proximo;
    }

    // Encontra um frame vazio
    for(int i=0; i<MAX_FRAMES; i++){
        if(alocacoes[i] == 0){
            pagina->frameIndex = i;
            return;
        }
    }
}

Page *CreatePage(int paginaID, int PID)
{
    Page *pagina = (Page *)malloc(sizeof(Page));

    pagina->paginaID = paginaID;
    pagina->PID = PID;
    pagina->frameIndex = -1;

    return pagina;
}

// Verifica se duas páginas são iguais
int Equal(Page *a, Page *b)
{
    return a->paginaID == b->paginaID && a->PID == b->PID;
}

// Verifica se duas páginas são iguais
int Equal2(Page *a, int paginaID, int PID)
{
    return a->paginaID == paginaID && a->PID == PID;
}
