#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;

typedef struct elem{
    int value;
    struct elem *prox;
}Elem;
 
typedef struct blockingQueue{
    unsigned int sizeBuffer, statusBuffer;
    Elem *head,*last;
}BlockingQueue;

// C = consumidores, P = produtores, B = Buffer Size
int C = 2, P = 2, B = 5;

BlockingQueue* bQueue;

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t myempty = PTHREAD_COND_INITIALIZER;
pthread_cond_t myfull = PTHREAD_COND_INITIALIZER;

BlockingQueue* newBlockingQueue(unsigned int SizeBuffer){
    BlockingQueue* newQueue = new BlockingQueue;

    newQueue->sizeBuffer = SizeBuffer;
    newQueue->statusBuffer = 0;
    newQueue->head = newQueue->last = NULL;
    
    return newQueue;
}

void putBlockingQueue(BlockingQueue* Q, int newValue){
    while (Q->statusBuffer == Q->sizeBuffer)
        pthread_cond_wait(&myempty, &mymutex);

    Elem *newElem = new Elem;

    newElem->value = newValue;
    newElem->prox = NULL;

    if (Q->head == NULL && Q->last == NULL)
        Q->head = Q->last = newElem;   
    
    else{
        Q->last->prox = newElem;
        Q->last = newElem;
    }

    Q->statusBuffer++;

    if (Q->statusBuffer == 1)
        pthread_cond_broadcast(&myfull); 
}

int takeBlockingQueue(BlockingQueue* Q){
    int ret;

    while (Q->statusBuffer == 0){
        cout << "Esta tentando consumir sem produto\n";
        pthread_cond_wait(&myfull, &mymutex);
    }
    
    Elem *delElem = Q->head;
    ret = delElem->value;

    Q->head = Q->head->prox;
    delete delElem;

    if (Q->head == NULL)
        Q->last = NULL;

    Q->statusBuffer--;

    if (Q->statusBuffer == Q->sizeBuffer - 1)
        pthread_cond_broadcast(&myempty); 


    return ret;
}

void *consumer(void *threadid) {
    int tid = *((int*) threadid);
    pthread_mutex_lock(&mymutex);
        cout << "Consumidor " << tid << " iniciou\n";
    pthread_mutex_unlock(&mymutex);
    
    // se quiser testar de uma maneira que de para verificar com poucos elementos, 
    // comente a linha 93 e descomente a linha 92, na funcao de producer tambem
    
    //for (int i = 0 ; i < 5 ; i++){
    while(1){
        pthread_mutex_lock(&mymutex);
        int v = takeBlockingQueue(bQueue);
            cout << "Consumidor " << tid << " consumiu " << v << "\n";
        pthread_mutex_unlock(&mymutex);
    }

    pthread_exit(NULL);
}

void *producer(void *threadid) {
    int tid = *((int*) threadid);
    pthread_mutex_lock(&mymutex);
        cout << "Produtor " << tid << " iniciou\n";
    pthread_mutex_unlock(&mymutex);

    // se quiser testar de uma maneira que de para verificar com poucos elementos,
    // substitua a condicao "1" por "i < 5"

    for (int i = 0 ; 1 ; i++){
        pthread_mutex_lock(&mymutex);
        putBlockingQueue(bQueue, i);
            cout << "Produtor " << tid << " produziu " << i << "\n";
        pthread_mutex_unlock(&mymutex);

    }

    pthread_exit(NULL);
}

int main(){
    bQueue = newBlockingQueue(B);

    pthread_t cThread[C];
    pthread_t pThread[P];
    int *cThread_ids[C];
    int *pThread_ids[P];
    int rc, i;

    for (i = 0 ; i < P ; i++){
        pThread_ids[i] = new int;
        *pThread_ids[i] = i;
        pthread_mutex_lock(&mymutex);
            cout << "Thread produtora " << i << " criada.\n";
        pthread_mutex_unlock(&mymutex);

        rc = pthread_create(&pThread[i], NULL, producer, (void *) pThread_ids[i]);
        
        if (rc){
            cout << "ERRO. Codigo de Retorno eh " << rc << endl; 
            exit(-1);
        }
    }

    for (i = 0 ; i < C ; i++){
        cThread_ids[i] = new int;
        *cThread_ids[i] = i;
        pthread_mutex_lock(&mymutex);
            cout << "Thread consumidora " << i << " criada.\n";
        pthread_mutex_unlock(&mymutex);

        rc = pthread_create(&cThread[i], NULL, consumer, (void *) cThread_ids[i]);
        
        if (rc){
            cout << "ERRO. Codigo de Retorno eh " << rc << "\n"; 
            exit(-1);
        }
    }

    for (i = 0 ; i < P ; i++)
        pthread_join(pThread[i], NULL);

    for (i = 0 ; i < C ; i++)
        pthread_join(cThread[i], NULL);

    delete bQueue;

    for (i = 0 ; i < P ; i++)
        delete pThread_ids[i];
  
    for(i = 0 ; i < C ; i++)
        delete cThread_ids[i];

    pthread_exit(NULL);
    return 0;
}