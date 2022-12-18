#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Estruturas.h"

int numProcessos = 0;

Processo *CriaProcesso(int PID)
{   
    printf("cria");
    Processo *processo = (Processo *)malloc(sizeof(Processo));

    processo->PID = numProcessos++;
    for (int i = 0; i < NUM_PAGINAS_PROCESSO; i++)
    {
        processo->tabelaPaginas[i] = (FilaElemento *)malloc(sizeof(FilaElemento)); // não está na memória principal
        processo->tabelaPaginas[i]->pagina = (Pagina *)malloc(sizeof(Pagina));
        processo->tabelaPaginas[i]->pagina->isInMP = 0;
    }
    processo->paginasNaMemoriaPrincipal = CriaFila(WORK_SET_LIMIT);
    return processo;
}

void InsereElementoNaTabelaDePaginas(Processo* processo, FilaElemento* elemento){
    processo->tabelaPaginas[elemento->pagina->paginaID] = elemento;
}

void AlocaPagina(Pagina* pagina, Fila* memoriaPrincipal){
    int alocacoes[NUM_FRAMES];
    memset(alocacoes, 0, sizeof(alocacoes));

    // Define a fila dos frames usados
    FilaElemento *p = memoriaPrincipal->primeiro;
    while (p != NULL)
    {
        if (p->pagina->frameIndex != -1)
            alocacoes[p->pagina->frameIndex] = 1;
        p = p->proximo;
    }

    // Encontra um frame vazio
    for(int i=0; i<NUM_FRAMES; i++){
        if(alocacoes[i] == 0){
            pagina->frameIndex = i;
            return;
        }
    }
}

Pagina *CriaPagina(int paginaID, int PID)
{
    Pagina *pagina = (Pagina *)malloc(sizeof(Pagina));

    pagina->paginaID = paginaID;
    pagina->PID = PID;
    pagina->frameIndex = -1;

    return pagina;
}

// Verifica se duas páginas são iguais
int Igual(Pagina *a, Pagina *b)
{
    return a->paginaID == b->paginaID && a->PID == b->PID;
}

// Verifica se duas páginas são iguais
int Igual2(Pagina *a, int paginaID, int PID)
{
    return a->paginaID == paginaID && a->PID == PID;
}
