
#define NUM_PAGINAS_PROCESSO 10
#define NUM_FRAMES 64
#define NUM_PROCESSOS 20
#define TAM_SWAP 500
#define WORK_SET_LIMIT 4

typedef struct _Page
{
    int paginaID;
    int PID;
    int isInMP;
    int frameIndex;                 // usado para identificar o frame de alocacao (ignorar nos outros locais)
} Page;

typedef struct _ElementQueue
{
    struct _ElementQueue *anterior;
    Page *pagina;
    struct _ElementQueue *proximo;
} ElementQueue;


typedef struct _Queue
{
    int size;
    int tamanhoMaximo;
    ElementQueue *primeiro;
} Queue;

typedef struct _Process
{
    int PID;
    ElementQueue *tabelaPaginas[NUM_PAGINAS_PROCESSO];
    Queue *paginasNaMemoriaPrincipal;
} Process;

Process *CreateProcess(int PID);
Queue *CreateQueue(int tamanhoMaximo);
Page *CreatePage(int paginaID, int PID);
ElementQueue *CreateElement(Queue *, Page *pagina);
ElementQueue *SearchElement(Queue *fila, Page *pagina);
ElementQueue *SearchElement2(Queue *fila, int paginaID, int PID);
void RemoveElement(Queue **fila, ElementQueue *elemento);
ElementQueue *RemoveFirst(Queue **fila);
int Possui(Queue *fila, ElementQueue *elemento);
int Equal(Page *a, Page *b);
int Equal2(Page *a, int paginaID, int PID);
ElementQueue *Insert(Queue **fila, ElementQueue *elemento);
void AllocPage(Page* pagina, Queue* memoriaPrincipal);
void InsertElementinTP(Process *processo, ElementQueue *elemento);