# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <time.h>



int main(int argc , char *argv[])
{
    if (argc != 2){
        printf("Error: Incorrect argument \n Usage: ./singlethread <filename>\n");
        return 1;}


    FILE *file = fopen(argv[1], "r");

    if (file == NULL){
        printf("Error: Can't open the file\n");
        return 1;}

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

    long long int sum = 0;
    int min,max ;

    clock_t start = clock();

    // printf("Hello World\n");
    

    for(int i = 0; i < data_len; i++){
        if (i == 0){
            min = data[i];
            max = data[i];
        }
        else{
            if (min > data[i]){
                min = data[i];
            }
            if (max < data[i]){
                max = data[i];
            }
        }
        
        sum += data[i];
    }

    // printf("Hello World\n");

    clock_t end = clock();

    printf("Sum of Dataset: %lld\n", sum);
    printf("Minimum: %d\n", min);
    printf("Maximum: %d\n", max);

    double time_taken = ((double)end - start)/CLOCKS_PER_SEC;
    printf("Time taken: %f seconds\n", time_taken);

    free(data);
    return 0;

}

