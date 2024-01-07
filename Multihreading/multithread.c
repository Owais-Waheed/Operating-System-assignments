# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <time.h>
# include <pthread.h>

// Structure to hold thread-specific data
struct thread_data {
    int* data;
    long long int start;
    long long int end;
    long long int part_sum;
    int part_min;
    int part_max;
};

// Initialize global sum, min, and max
    long long int global_sum = 0;
    int global_min = 1e9;
    int global_max = 0;

// Mutexes and condition variables for synchronization
pthread_mutex_t sum_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t min_max_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to be executed by each worker thread
void* thread_worker(void* arg) {
    struct thread_data* thread_data = (struct thread_data*)arg;

    // Initialize partial sum, min, and max
    thread_data->part_sum = 0;
    thread_data->part_min = thread_data->data[thread_data->start];
    thread_data->part_max = thread_data->data[thread_data->start];

    // Process the assigned chunk of data
    for (long long int i = thread_data->start; i < thread_data->end; i++) {
        thread_data->part_sum += thread_data->data[i];

        if (thread_data->part_min > thread_data->data[i]) {
            thread_data->part_min = thread_data->data[i];
        }

        if (thread_data->part_max < thread_data->data[i]) {
            thread_data->part_max = thread_data->data[i];
        }
    }

    // Acquire lock and update global sum with partial sum
    pthread_mutex_lock(&sum_mutex);
    // Update the global sum
    global_sum += thread_data->part_sum;
    pthread_mutex_unlock(&sum_mutex);

    // Acquire lock and update global min and max with partial min and max
    pthread_mutex_lock(&min_max_mutex);
    // Update the global min and max
    if (global_min > thread_data->part_min) {
        global_min = thread_data->part_min;
    }

    if (global_max < thread_data->part_max) {
        global_max = thread_data->part_max;
    }
    pthread_mutex_unlock(&min_max_mutex);

    return NULL;
}


int main(int argc , char *argv[])
{
    if ((argc < 2) || (argc > 3)){
        printf("Error: Incorrect argument \n Usage: ./multithread <filename> <numthreads / optional>\n");
        return 1;}

    int num_threads;

    if (argc == 3){
        num_threads = atoi(argv[2]);
    }
    else{
        num_threads = 4;
        printf("No. of threads not given. Default value of 4 is used.\n");
    }

    if (num_threads < 1){
        printf("Error: Invalid number of threads\n");
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    
    if (file == NULL){
        printf("Error: Can't open the file\n");
        return 1;
    }

    long long int data_len = 0;
    int curr;
    int limit = 1000;

    int *data = malloc(1000 * sizeof(int)); // I am using int because it's max value is 2^31 - 1 which is more than enough for the data set

    if (data == NULL){
        printf("Error: Memory allocation failed\n");
        return 1;}


    while((fscanf(file, "%d", &curr) == 1) & (feof(file) == 0) ){
        data[data_len] = curr;
        data_len++;


        if (data_len == limit){
            limit = limit * 2;
            data = realloc(data, limit * sizeof(int));

            if (data == NULL){
                printf("Error: Memory reallocation failed\n");
                return 1;}
        }
        
    }

    fclose(file);

    

    // Create an array to hold thread IDs
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));

    // Initialize thread-specific data
    struct thread_data* thread_arr = malloc(num_threads * sizeof(struct thread_data));
    long long int chunk_size = data_len / num_threads;
    long long int remainder = data_len % num_threads;


    
    struct timespec start , end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    // Create and launch worker threads
    for (int i = 0; i < num_threads; i++) {
        thread_arr[i].data = data;
        thread_arr[i].start = i * chunk_size;
        thread_arr[i].end = (i == num_threads - 1) ? (i + 1) * chunk_size + remainder : (i + 1) * chunk_size;

        pthread_create(&threads[i], NULL, thread_worker, &thread_arr[i]);
    }

    // Wait for all threads to finish
    for (int i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    // Print results
    printf("Sum of Dataset: %lld\n", global_sum);
    printf("Minimum: %d\n", global_min);
    printf("Maximum: %d\n", global_max);

    double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("Main Thread Time taken: %f seconds\n", time_taken);

    // Clean up
    free(data);
    free(threads);
    free(thread_arr);

    return 0;
    

}