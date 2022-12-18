#include <stdlib.h>
#include <stdio.h>

#include "Estruturas.h"

// Cria uma referencia para nova fila
Fila *CriaFila(int tamanhoMaximo)
{
    Fila *fila = (Fila *)malloc(sizeof(Fila));
    fila->primeiro = NULL;
    fila->size = 0;
    fila->tamanhoMaximo = tamanhoMaximo;
    return fila;
}


// Busca um elemento da fila
FilaElemento *BuscaElemento(Fila *fila, Pagina *pagina)
{
    FilaElemento *p = fila->primeiro;
    while (p != NULL && !Igual(p->pagina, pagina))
        p = p->proximo;

    return p;
}

// Cria um elemento de fila
FilaElemento *CriaElemento(Fila *fila, Pagina *pagina)
{
    FilaElemento *elemento = BuscaElemento(fila, pagina);
    if (elemento == NULL)
    {
        elemento = (FilaElemento *)malloc(sizeof(FilaElemento));
        elemento->pagina = pagina;
        elemento->anterior = NULL;
        elemento->proximo = NULL;
    }
    return elemento;
}

// Assumindo que o elemento está na fila, move-o para o final dela
void MoveElementoParaOFinal(Fila **fila, FilaElemento *elemento)
{
    // Se o elemento já não estiver no final
    if (elemento->proximo != NULL)
    {
        FilaElemento *p = (*fila)->primeiro;
        FilaElemento *q = p->proximo;

        // Se o primeiro elemento for o desejado
        if (Igual(p->pagina, elemento->pagina))
        {
            (*fila)->primeiro = q;
            while (q->proximo != NULL)
                q = q->proximo;
            q->proximo = p;
            p->anterior = q;
        }
        else
        {
            while (q != elemento)
            {
                p = p->proximo;
                q = q->proximo;
            }
            p->proximo = q->proximo;
            if (q->proximo != NULL)
                q->proximo->anterior = p;
            while (q->proximo != NULL)
            {
                q = q->proximo;
            }
            q->proximo = elemento;
            elemento->anterior = q;
        }
    }

    elemento->proximo = (*fila)->primeiro;
    (*fila)->primeiro->anterior = elemento;
}

// Insere um elemento de fila
FilaElemento *Insere(Fila **fila, FilaElemento *elemento)
{
    // printf("Fila: (%d,%d)", (*fila)->size, (*fila)->tamanhoMaximo);
    FilaElemento *elementoRemovido = (FilaElemento *)NULL;
    if (Possui(*fila, elemento))
    {
        MoveElementoParaOFinal(fila, elemento);
    }
    else
    {
        if ((*fila)->primeiro == NULL)
        {
            (*fila)->primeiro = elemento;
            elemento->anterior = NULL;
            elemento->proximo = NULL;
        }
        else
        {
            FilaElemento *p = (*fila)->primeiro;
            while (p->proximo != NULL)
            {
                p = p->proximo;
            }
            p->proximo = elemento;
            elemento->anterior = p;
            elemento->proximo = NULL;
        }

        if ((*fila)->size == (*fila)->tamanhoMaximo)
        {
            elementoRemovido = RemovePrimeiro(fila);
        }
        else
        {
            (*fila)->size++;
        }
    }

    return elementoRemovido;
}

// Busca um elemento da fila
FilaElemento *BuscaElemento2(Fila *fila, int paginaID, int PID)
{
    FilaElemento *p = fila->primeiro;
    while (p != NULL && !Igual2(p->pagina, paginaID, PID))
        p = p->proximo;

    return p;
}

// Verifica se fila possui elemento
int Possui(Fila *fila, FilaElemento *elemento)
{
    int filaPossuiElemento = 0;
    FilaElemento *p = fila->primeiro;
    if (p == NULL)
        return 0;
    while (p->proximo != NULL)
    {
        if (Igual(p->pagina, elemento->pagina))
            filaPossuiElemento = 1;
        p = p->proximo;
    }

    if (Igual(p->pagina, elemento->pagina))
        filaPossuiElemento = 1;

    return filaPossuiElemento;
}

FilaElemento *RemovePrimeiro(Fila **fila)
{
    FilaElemento *p = (*fila)->primeiro;
    (*fila)->primeiro = p->proximo;
    (*fila)->primeiro->anterior = NULL;
    return p;
}

// Remove um elemento de fila
void RemoveElemento(Fila **fila, FilaElemento *elemento)
{
    if (Possui(*fila, elemento))
    {
        FilaElemento *p = (*fila)->primeiro;
        if (Igual(p->pagina, elemento->pagina))
        {
            p = RemovePrimeiro(fila);
            free(p);
        }
        else
        {
            FilaElemento *q = p->proximo;
            while (q != elemento)
            {
                p = p->proximo;
                q = q->proximo;
            }
            p->proximo = q->proximo;
            if (q->proximo != NULL)
                q->proximo->anterior = p;
            free(q);
        }
        (*fila)->size--;
    }
}