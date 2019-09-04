#include <mpi.h>
#include <stdio.h>
#include <unistd.h>

typedef struct pair{
    float first;
    float second;
} Pair;

Pair get_pair(float a, float b){
    Pair out = {a, b};
    return out;
}

Pair compare(Pair input){
    if(input.first >= input.second){
        Pair out = {input.second, input.first};
        return out;
    }else{
        Pair out = {input.first, input.second};
        return out;
    }
}



int main(int argc, char *argv[]){
    printf("Hello World\n");

    // printf("%s\n", argv[0]);
    // printf("%s\n", argv[1]);
    // printf("%s\n", argv[2]);

    int len = atoi(argv[1]);
    // int input_file_len = sizeof(argv[2]);
    // int output_file_len = sizeof(argv[3]);
    char* input_file = argv[2];
    char* output_file = argv[3];
    float* seq = malloc(sizeof(float) * len);
    float* coll = malloc(sizeof(float) * len);
    int rank = -1, size = -1;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Here is proccess %d\n", rank);



}