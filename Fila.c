#include <stdlib.h>
#include <stdio.h>
#include "Estruturas.h"


// Cria uma fila
Queue *CreateQueue(int tamanhoMaximo)
{
    Queue *fila = (Queue *)malloc(sizeof(Queue));
    fila->primeiro = NULL;
    fila->size = 0;
    fila->tamanhoMaximo = tamanhoMaximo;
    return fila;
}


// Procura um elemento da fila
ElementQueue *SearchElement(Queue *fila, Page *pagina)
{
    ElementQueue *p = fila->primeiro;
    while (p != NULL && !Equal(p->pagina, pagina))
        p = p->proximo;

    return p;
}

// Cria um elemento
ElementQueue *CreateElement(Queue *fila, Page *pagina)
{
    ElementQueue *elemento = SearchElement(fila, pagina);
    if (elemento == NULL)
    {
        elemento = (ElementQueue *)malloc(sizeof(ElementQueue));
        elemento->pagina = pagina;
        elemento->anterior = NULL;
        elemento->proximo = NULL;
    }
    return elemento;
}

// Move o elemento para o final da fila
void MoveElementoParaOFinal(Queue **fila, ElementQueue *elemento)
{
    // Se o elemento não estiver no final
    if (elemento->proximo != NULL)
    {
        ElementQueue *p = (*fila)->primeiro;
        ElementQueue *q = p->proximo;

        // Se o primeiro elemento for o desejado
        if (Equal(p->pagina, elemento->pagina))
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

ElementQueue *RemoveFirst(Queue **fila)
{
    ElementQueue *p = (*fila)->primeiro;
    if(p->proximo != NULL)
    (*fila)->primeiro = p->proximo;
    (*fila)->primeiro->anterior = NULL;
    return p;
}

// Insere um elemento na fila
ElementQueue *Insert(Queue **fila, ElementQueue *elemento)
{
    ElementQueue *elementoRemovido = (ElementQueue *)NULL;
    if (IsInQueue(*fila, elemento))
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
            ElementQueue *p = (*fila)->primeiro;
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
            elementoRemovido = RemoveFirst(fila);
        }
        else
        {
            (*fila)->size++;
        }
    }

    return elementoRemovido;
}

// Procura um elemento
ElementQueue *SearchElement2(Queue *fila, int paginaID, int PID)
{
    ElementQueue *p = fila->primeiro;
    while (p != NULL && !Equal2(p->pagina, paginaID, PID))
        p = p->proximo;

    return p;
}

// Verifica se elemento está na fila
int IsInQueue(Queue *fila, ElementQueue *elemento)
{
    int filaPossuiElemento = 0;
    ElementQueue *p = fila->primeiro;
    if (p == NULL)
        return 0;
    while (p->proximo != NULL)
    {
        if (Equal(p->pagina, elemento->pagina))
            filaPossuiElemento = 1;
        p = p->proximo;
    }

    if (Equal(p->pagina, elemento->pagina))
        filaPossuiElemento = 1;

    return filaPossuiElemento;
}



// Remove um elemento de fila
void RemoveElement(Queue **fila, ElementQueue *elemento)
{
    if (IsInQueue(*fila, elemento))
    {
        ElementQueue *p = (*fila)->primeiro;
        if (Equal(p->pagina, elemento->pagina))
        {
            p = RemoveFirst(fila);
            free(p);
        }
        else
        {
            ElementQueue *q = p->proximo;
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