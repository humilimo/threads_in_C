#include <bits/stdc++.h>
#include <pthread.h>

#define NUM_THREADS 10

using namespace std;

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

int cont = 0;

void *incrementador(void *thread_ids){
    int tid = *((int*)thread_ids);

    while (1){
        pthread_mutex_lock(&mymutex);
        if (cont < 1000000){
            cont++;
            if (cont == 1000000)
                cout << "A thread " << tid << " chegou ao valor " << cont << "\n";
        }
        pthread_mutex_unlock(&mymutex);
        if (cont == 1000000)
            break;
    }
    pthread_exit(NULL);
}

int main(){
    pthread_t threads[NUM_THREADS];
    int *thread_ids[NUM_THREADS];
    int rc, i;

    for (i = 0 ; i < NUM_THREADS ; i++){
        thread_ids[i] = new int;
        *thread_ids[i] = i;

        rc = pthread_create(&threads[i], NULL, incrementador, (void *) thread_ids[i]);
        
        if (rc){
            cout << "ERRO. Codigo de Retorno eh " << rc << "\n";
            exit(-1);
        }
    }

    for (i = 0 ; i < NUM_THREADS ; i++)
        pthread_join(threads[i], NULL);

    for(i = 0; i < NUM_THREADS; i++)
        delete thread_ids[i];

    pthread_exit(NULL);

    return 0;
}