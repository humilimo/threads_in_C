#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;

struct func{
    int n1, n2;

    int funexec(int n1, int n2){
        return n1 * n2;
    }
};

struct paramThread{
    int i;
    func f;
    int ID;
};

int MAX_THREADS, NUM_THREADS = 0, ID = 0;
pthread_mutex_t bufferMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t resultMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t notReady = PTHREAD_COND_INITIALIZER;
pthread_cond_t bufferEmpty = PTHREAD_COND_INITIALIZER;
pthread_cond_t threadFull = PTHREAD_COND_INITIALIZER;

pthread_t ThreadDespachante;
vector <pair<pthread_t, bool>> Threads;

set <int> InvIds;

queue <pair<func, int>> buffer;
unordered_map <int, pair<int, bool>> results;  // hashtable com chave = id, elemento = resultado

void agendarExecucao(func param){
    pthread_mutex_lock(&bufferMutex);

    func f = param;

    buffer.push(pair<func, int>(f, ID));

    results[ID] = pair<int, bool>(0, 0);
    
    ID++;

    if (buffer.size() == 1)
        pthread_cond_broadcast(&bufferEmpty); 

    pthread_mutex_unlock(&bufferMutex);
}

int pegarResultadoExecucao(int id){
    pthread_mutex_lock(&threadMutex);
    while(results[id].second == 0)        
        pthread_cond_wait(&notReady, &threadMutex);
    pthread_mutex_unlock(&threadMutex);

    return results[id].first;
}

void* threadFunc(void* param){
    pthread_mutex_lock(&threadMutex);

    paramThread p = *((paramThread*) param);

    int result = p.f.funexec(p.f.n1, p.f.n2);
    
    results[p.ID].first = result;
    results[p.ID].second = 1;

    pthread_cond_broadcast(&notReady); 
    pthread_cond_broadcast(&threadFull);
    Threads[p.i].second = 0;                                                                   
    NUM_THREADS--;

    pthread_mutex_unlock(&threadMutex);

    pthread_exit(NULL);
}                                                                   
                                                                   
void* funcDesp(void* param){                                                                   
    while(1){                                                                   
        pthread_mutex_lock(&bufferMutex);

        while (buffer.empty())
            pthread_cond_wait(&bufferEmpty, &bufferMutex);                                              
                                                                   
        pthread_mutex_lock(&threadMutex);

        while (NUM_THREADS == MAX_THREADS)                                                                   
            pthread_cond_wait(&threadFull, &threadMutex);

        int i, rc;                                                                   
                                                                   
        for (i = 0 ; i < MAX_THREADS ; i++)                                                                   
            if (Threads[i].second == 0)                                                                   
                break;                                                                   
                                                                   
        Threads[i].second = 1;                                                                   
        NUM_THREADS++;  

        paramThread* p = new paramThread;

        p->ID = buffer.front().second;
        p->f = buffer.front().first;
        buffer.pop();
        p->i = i;

        rc = pthread_create(&(Threads[i].first), NULL, threadFunc, (void*) p);                                                                   
                                                                   
        if (rc){                                                                   
            cout << "ERRO. Codigo de Retorno eh " << rc << endl;                                                                    
            exit(-1);                                                                   
        }                                                                   
        pthread_mutex_unlock(&bufferMutex);
        pthread_mutex_unlock(&threadMutex);       
    }                                                                   
}                                                                   
                                                                   
                                                                   
int main(){  
    cout << "Quantos nucleos/processadores tem o SO? ";

    cin >> MAX_THREADS;

    cout << "\n";
    Threads.resize(MAX_THREADS);
    int comm;

    int rc = pthread_create(&ThreadDespachante, NULL, funcDesp, NULL);

    if (rc){                                                                   
        cout << "ERRO. Codigo de Retorno eh " << rc << endl;                                                                    
        exit(-1);                                                                   
    }        

    for(int i = 0 ; 1 ; i++){
        cout << "---------------------MENU---------------------\n";
        cout << "(1) Agendar Execucao de uma Funcao\n";
        cout << "(2) Pegar Resultado da Execucao de uma Funcao\n";
        cout << "----------------------------------------------\n";

        cin >> comm;

        if (comm == 1){
            func param;
            param.n1 = ID;
            param.n2 = ID + 1;

            agendarExecucao(param);
        }

        else if (comm == 2){
            int id;
            
            cout << "Qual o ID da funcao que voce quer o resultado? ";
            cin >> id;

            if (id >= ID)
                cout << "ID " << id << " Invalido.\n";
            
            else if (InvIds.count(id))
                cout << "Funcao de ID " << id << " ja foi executada.\n";

            else{
                InvIds.insert(id);
                cout << "O resultado da funcao de ID " << id << " eh " << pegarResultadoExecucao(id) << "\n";
            }

        }
    }

    pthread_exit(NULL);
    return 0;
}