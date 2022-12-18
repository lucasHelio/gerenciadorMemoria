
#define NUM_PAGINAS_PROCESSO 10
#define NUM_FRAMES 64
#define NUM_PROCESSOS 20
#define TAM_SWAP 500
#define WORK_SET_LIMIT 4

typedef struct _Pagina
{
    int paginaID;
    int PID;
    int isInMP;
    int frameIndex;                 // usado para identificar o frame de alocacao (ignorar nos outros locais)
} Pagina;

typedef struct _FilaElemento
{
    struct _FilaElemento *anterior;
    Pagina *pagina;
    struct _FilaElemento *proximo;
} FilaElemento;


typedef struct _Fila
{
    int size;
    int tamanhoMaximo;
    FilaElemento *primeiro;
} Fila;

typedef struct _Processo
{
    int PID;
    FilaElemento *tabelaPaginas[NUM_PAGINAS_PROCESSO];
    Fila *paginasNaMemoriaPrincipal;
} Processo;

Processo *CriaProcesso(int PID);
Fila *CriaFila(int tamanhoMaximo);
Pagina *CriaPagina(int paginaID, int PID);
FilaElemento *CriaElemento(Fila *, Pagina *pagina);
FilaElemento *BuscaElemento(Fila *fila, Pagina *pagina);
FilaElemento *BuscaElemento2(Fila *fila, int paginaID, int PID);
void RemoveElemento(Fila **fila, FilaElemento *elemento);
FilaElemento *RemovePrimeiro(Fila **fila);
int Possui(Fila *fila, FilaElemento *elemento);
int Igual(Pagina *a, Pagina *b);
int Igual2(Pagina *a, int paginaID, int PID);
FilaElemento *Insere(Fila **fila, FilaElemento *elemento);
void AlocaPagina(Pagina* pagina, Fila* memoriaPrincipal);
void InsereElementoNaTabelaDePaginas(Processo *processo, FilaElemento *elemento);