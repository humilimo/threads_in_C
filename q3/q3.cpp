#include <bits/stdc++.h>
#include <pthread.h>

using namespace std;

typedef struct {
    int R, G, B;
} pixel;

typedef struct{
    char magicNumber[1000000];
    int height, width;
    int max;
    vector<vector<pixel>> image, gray;
} ppm;

int NUM_THREADS;
ppm file; 

void readFile(){
    FILE *f = fopen("file.ppm", "r+b");

    if(f == NULL){
        cout << "Erro ao abrir o arquivo\n";
        exit(1);
    }
 
    fscanf(f, "%s", file.magicNumber);

    fscanf(f, "%d %d", &(file.width), &(file.height));

    fscanf(f, "%d", &(file.max));

    NUM_THREADS = file.width * file.height;

    file.image.resize(file.height, vector<pixel>(file.width));
    file.gray.resize(file.height, vector<pixel>(file.width));
    
    int row = 0, column = 0;
    
    for (int i = 0 ; i < file.height ; i++)
        for (int j = 0 ; j < file.width ; j++)
            fscanf(f, "%d %d %d", &(file.image[i][j].R), &(file.image[i][j].G), &(file.image[i][j].B));

    fclose(f);
}

void* convertColour(void* thread_id){
    int tid = *((int*) thread_id);
    int row = tid / file.width;
    int column = tid % file.width;

    float red = (float) file.image[row][column].R;
    float green = (float) file.image[row][column].G;
    float blue = (float) file.image[row][column].B;

    file.gray[row][column].R = file.gray[row][column].G = file.gray[row][column].B = (int) (red * 0.30 + green * 0.59 + blue * 0.11);

    pthread_exit(NULL);
}

void writeFile(){
    FILE *f = fopen("newfile.ppm", "w+b");

    if(f == NULL){
        cout << "Erro ao abrir o arquivo\n";
        exit(1);
    }

    fprintf(f, "%s\n", file.magicNumber);
    fprintf(f, "%d %d\n", file.width, file.height);
    fprintf(f, "%d\n", file.max);

    for (int i =  0 ; i < file.height ;  i++)
        for (int j = 0 ; j < file.width ; j++)
            fprintf(f, "%d %d %d\n", file.gray[i][j].R,  file.gray[i][j].G,  file.gray[i][j].B);

    cout << "Arquivo escrito com sucesso\n";
    fclose(f);
}

int main(){
    readFile();

    pthread_t threads[NUM_THREADS];
    int *thread_ids[NUM_THREADS];
    int rc, i;

    for (i = 0 ; i < NUM_THREADS ; i++){
        thread_ids[i] = new int;
        *thread_ids[i] = i;
        
        rc = pthread_create(&threads[i], NULL, convertColour, (void *) thread_ids[i]);
        
        if (rc){
            cout << "ERRO. Codigo de Retorno eh " << rc << "\n"; 
            exit(-1);
        }
    }

    for (i = 0 ; i < NUM_THREADS ; i++)
        pthread_join(threads[i], NULL);

    writeFile();

    for(i = 0; i < NUM_THREADS; i++)
        delete thread_ids[i];

    pthread_exit(NULL);

    return 0;
}