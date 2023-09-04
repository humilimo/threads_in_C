#include <bits/stdc++.h>
#include <pthread.h>

#define NUM_VAR 4

using namespace std;

pthread_barrier_t barrier;

int NUM_THREADS;

int a[NUM_VAR][NUM_VAR] = {{4,  2,  1, -2},
                           {3, -3, -1, -1},
                           {3,  5,  1,  1}, 
                           {1, -1, -1,  4}};

int b[NUM_VAR] = {3, 2, 0, -2}; 

vector <int> k;
const int p = 5;
vector <vector<float>> result(p+1, vector<float> (NUM_VAR, 1));

void *jacobi(void *thread_ids){
    int tid = *((int*)thread_ids);
    
    while(k[tid] <= p){      
        for(int i = tid ; i < NUM_VAR ; i += NUM_THREADS){

            float sum = 0;
            
            for(int j = 0 ; j < NUM_VAR ; j++)
                if (j != i)
                    sum += (float) (a[i][j] * result[k[tid] - 1][j]);
            
            result[k[tid]][i] = (float) ((b[i] - sum) / a[i][i]);

        }
        pthread_barrier_wait(&barrier);
        k[tid]++;
    }
    pthread_exit(NULL);
}

int nbrDigits(int n){
    if (n < 0)
        n *= -1;

    int result = 0;

    while (n >= 10){
        n /= 10;
        result++;
    }

    return result;
}

void printMatriz(){
    for (int i = 0 ; i <= p ; i++){
        cout << "Iteracao "<< i << ": |";
        
        for (int j = 0 ; j < NUM_VAR ; j++){
            int num = nbrDigits((int) result[i][j]);

            if (result[i][j] < 0)
                cout << fixed << setprecision(7 - num);
            else
                cout << fixed << setprecision(8 - num);

            cout << " x" << j + 1 << " = " << result[i][j] << " |";
        }

        cout << "\n";
    }
}

int main(){
    cout << "Digite quantos processadores/nucleos tem sua maquina: ";
    cin >> NUM_THREADS;
    k.resize(NUM_THREADS, 1);
    
	pthread_barrier_init(&barrier, NULL, NUM_THREADS);
    pthread_t threads[NUM_THREADS];
    int *thread_ids[NUM_THREADS];
    int rc, i;

    for (i = 0 ; i < NUM_THREADS ; i++){
        thread_ids[i] = new int;
        *thread_ids[i] = i;

        rc = pthread_create(&threads[i], NULL, jacobi, (void *) thread_ids[i]);
        
        if (rc){
            cout << "ERRO. Codigo de Retorno eh " << rc << endl; 
            exit(-1);
        }
    }

    for (i = 0 ; i < NUM_THREADS ; i++)
        pthread_join(threads[i], NULL);

    printMatriz();

    for(i = 0; i < NUM_THREADS; i++)
        delete thread_ids[i];


    pthread_barrier_destroy(&barrier);
    pthread_exit(NULL);

    return 0;
}