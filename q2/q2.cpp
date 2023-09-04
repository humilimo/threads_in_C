#include <bits/stdc++.h>
#include <pthread.h>

#define NUM_THREADS 5

using namespace std;

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;
vector < vector <int> > graph(NUM_THREADS), cycles(NUM_THREADS);
bool visited[NUM_THREADS][NUM_THREADS] = {0};
bool cycleDetected = false;

void createGraph(){
    /* Grafo usado no exemplo
    
        0 -> 1 -> 2 
             ^    |
             |    v
             4 <- 3
    */
    graph[0].push_back(1); // 0 -> 1
    graph[1].push_back(2); // 1 -> 2
    graph[2].push_back(3); // 2 -> 3
    graph[3].push_back(4); // 3 -> 4
    graph[4].push_back(1); // 4 -> 1

    /*  Caso o senhor queira testar com outros grafos, siga esse modelo de insercao de
        vertice na lista de adjacencias:
        
        - graph[ORIGEM].pushback(DESTINO); 
        
        - E altera na linha 4 o NUM_THREADS para ser igual o numero de vertices, se nao vai dar segmentation fault.

        Caso queira um caso simples sem deadlock, basta comentar a ultima insercao feita (linha 23);
    */
}
    
void printCycle(int tid){
    cout << "A thread " << tid << " encontrou o seguinte ciclo: " << tid << "->";
    for(auto x : cycles[tid])
         cout << x << "->";
    cout << tid << "\n";
}

void dfs(int tid, int v){
    visited[tid][v] = 1;

    for(auto x : graph[v]){
        if(x == tid){
            pthread_mutex_lock(&mymutex);
                printCycle(tid);
                cycleDetected = 1;
            pthread_mutex_unlock(&mymutex);
        }
        if(!visited[tid][x]){
            cycles[tid].push_back(x);
            dfs(tid, x);
        }
    }
}

void *callDFS(void* thread_id){
    int tid = *((int*) thread_id);
    dfs(tid, tid);
    pthread_mutex_lock(&mymutex);
        cout << "Thread " << tid << " terminada\n";
    pthread_mutex_unlock(&mymutex);
    pthread_exit(NULL);
}

int main(){
    createGraph();

    pthread_t threads[NUM_THREADS];
    int *thread_ids[NUM_THREADS];
    int rc, i;

    for (i = 0 ; i < NUM_THREADS ; i++){
        thread_ids[i] = new int;
        *thread_ids[i] = i;

        pthread_mutex_lock(&mymutex);
            cout << "Thread " << i << " criada\n";
        pthread_mutex_unlock(&mymutex);
        
        rc = pthread_create(&threads[i], NULL, callDFS, (void *) thread_ids[i]);
        
        if (rc){
            cout << "ERRO. Codigo de retorno eh " << rc << "\n";
            exit(-1);
        }
    }

    for (i = 0 ; i < NUM_THREADS ; i++)
        pthread_join(threads[i], NULL);

    if(!cycleDetected)
        cout << "Nenhum ciclo foi detectado, portanto, nao ocorrera deadlock\n";
    else
        cout << "Portanto, podera ocorrer deadlock\n";

    for(i = 0; i < NUM_THREADS; i++)
        delete thread_ids[i];

    pthread_exit(NULL);

    return 0;
}