#include <stdio.h>
#include <unistd.h>
// #include <mpi.h>

typedef enum{
    true = 1 == 1,
    false = 1 == 0
}bool;

void compare(float *a, float *b){
    if(*a > *b){
        float t = *a;
        *a = *b;
        *b = t;
    }
}

int open_file(int len, char *file_in, FILE **pfile_in, char *file_out, FILE **pfile_out, float **seq_in, float **seq_out){
    *pfile_in = fopen(file_in, "r");
    *pfile_out = fopen(file_out, "w");

    if(*pfile_in == NULL || *pfile_out == NULL){
        return 0;
    }

    *seq_in = malloc(sizeof(float) * len);
    *seq_out = malloc(sizeof(float) * len);

    if(*seq_in == NULL || *seq_out == NULL){
        return 0;
    }

    for(int i = 0; i<len; i++){
        float t;
        fscanf(*pfile_in, "%f", &t);
        
        (*seq_in)[i] = t;
        (*seq_out)[i] = t;
        // printf("%f, %f\n", (*seq_in)[i], (*seq_out)[i]);
    }
    return 1;
}

void swap(int rank, int start, int end, float *seq_out){
    // printf("0\n");
    int len = end - start + 1;
    // printf("1\n");
    if(len % 2 == 0){
        // printf("2\n");
        for(int i = start; i <= end; i+=2){
            // printf("3\n");
            compare(seq_out+i, seq_out+i+1);
            // printf("4\n");
            printf("%f %f ", seq_out[i], seq_out[i+1]);
        }
    }
    
}

void odd_phase(){

}

void even_phase(int rank, int comm_size, int seq_len, float *seq_out){
    int pair_num = seq_len / 2;
    int segment_len_longer = (pair_num / comm_size + 1) * 2;
    int segment_len_shorter = (pair_num / comm_size) * 2;

    int segment_len = segment_len_shorter; // For remain_pairs == 0
    int remain_pairs = pair_num % comm_size;
    
    if(remain_pairs != 0){
        if(rank < remain_pairs){
            segment_len = segment_len_longer;
        }
    }
    printf("segment: %d, pair_num: %d, segment_longer: %d, segment_shorter: %d\n", segment_len, pair_num, segment_len_longer, segment_len_shorter);
    
    int start = rank <= remain_pairs? rank * segment_len_longer : remain_pairs * segment_len_longer + (rank - remain_pairs) * segment_len_shorter;
    int end = start + segment_len - 1;

    printf("Rank %d, start: %d, End: %d in Comm_size=%d, Seq_len=%d\n", rank, start, end, comm_size, seq_len);
    swap(rank, start, end, seq_out);
}

void sort(int rank, int comm_size, int seq_len, int *seq_out){

}

void monitor(){
    // comm_size should < seq_len/2
}

// Return is continue or not?
bool wait_sync(int rank, bool is_even_swap, bool is_odd_swap, int *msg_arr, int comm_size, MPI_Comm communicator){
    int collector = comm_size - 1;
    // int *msg_arr = malloc(sizeof(int) * comm_size); // Move to Global
    // msg_arr = 0: Not yet or Continue
    //         = 1: Done, but swap pair during proccessing
    //         = 2: Done, No swap pair during proccessing
    //         = 3: Request Exit
    if(comm_size == 1){

    }else{
        if(rank == collector){
            bool is_sort_finish = true;
            for(int i=0; i<comm_size-1; i++){
                MPI_Recv(msg_arr + i, 1, MPI_INT, i, 0, communicator, MPI_STATUS_IGNORE);
                if(msg_arr[i] == 1){is_sort_finish = false;}
            }
            if(!is_sort_finish){
                for(int i=0; i<comm_size-1; i++){
                    msg_arr[i] = 0;
                    MPI_Send(msg_arr + i, 1, MPI_INT, i, 1, communicator);
                }
                return true;
            }else{
                for(int i=0; i<comm_size-1; i++){
                    msg_arr[i] = 3;
                    MPI_Send(msg_arr + i, 1, MPI_INT, i, 1, communicator);
                }
                return false;
            }
        }else{// Sender
            msg_arr[rank] = is_even_swap || is_odd_swap? 1:2;
            MPI_Send(msg_arr + rank, 1, MPI_INT, collector, 0, communicator);
            MPI_Recv(msg_arr + rank, 1, MPI_INT, collector, 1, communicator, MPI_STATUS_IGNORE);
            if(msg_arr[rank] == 0){return true;}
            else if(msg_arr[rank] == 4){return false;}
        }
    }
}

int close_file(FILE *pfile_in, FILE *pfile_out){
    fclose(pfile_in);
    fclose(pfile_out);
}

FILE *pfile_in = NULL;
FILE *pfile_out = NULL;
float *seq_in = NULL;
float *seq_out = NULL;

int main(int argc, char* argv[]){

    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);
    printf("%s\n", argv[3]);

    open_file(atoi(argv[1]), argv[2], &pfile_in, argv[3], &pfile_out, &seq_in, &seq_out);

    
    int comm_size = 4;
    int seq_len = atoi(argv[1]);
    printf("Comm_size: %d, Seq_len: %d\n", comm_size, seq_len);
    for(int i = 0; i<comm_size; i++){
        even_phase(i, comm_size, seq_len, seq_out);
        
    }
    
}