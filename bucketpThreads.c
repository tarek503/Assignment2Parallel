#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define MAX_THREADS 3

typedef struct {
    int* bucket;
    int size;
} SortArgs;

typedef struct {
    int* arr;
    int start, end;
    int min, rangePerBucket;
    int** buckets;
    int* sizes;
} FillArgs;

void insertionSort(int* arr, int n) {
    for (int i = 1; i < n; i++) {
        int temp = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > temp) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = temp;
    }
}
int getMin(int* arr, int n) {
    int min = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] < min) min = arr[i];
    }
    return min;
}

int getMax(int* arr, int n) {
    int max = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > max) max = arr[i];
    }
    return max;
}

void* fillBucket(void* args) {
    FillArgs* fa = (FillArgs*)args;
    for (int i = fa->start; i < fa->end; i++) {
        int bi = (fa->arr[i] - fa->min) / fa->rangePerBucket; 
        fa->buckets[bi][fa->sizes[bi]++] = fa->arr[i];
    }
    pthread_exit(NULL);
}

void* sortBucket(void* args) {
    SortArgs* sa = (SortArgs*)args;
    insertionSort(sa->bucket, sa->size);
    pthread_exit(NULL);
}

void bucketSort(int* arr, int n) {
    int max = getMax(arr, n);
    int min = getMin(arr, n);
    int range = max - min + 1;
    int nbBuckets = sqrt(n);
    int rangePerBucket = range / nbBuckets + 1;

    int** buckets = (int**)malloc(nbBuckets * sizeof(int*));
    int* sizes = (int*)calloc(nbBuckets, sizeof(int));

    for (int i = 0; i < nbBuckets; i++) {
        buckets[i] = (int*)malloc(n * sizeof(int));
    }

  
    pthread_t fillThreads[MAX_THREADS];
    FillArgs fillArgs[MAX_THREADS];
    int len = n / MAX_THREADS;
    for (int i = 0; i < MAX_THREADS; i++) {
        fillArgs[i] = (FillArgs){arr, i * len, (i + 1) * len, min, rangePerBucket, buckets, sizes};
        if (i == MAX_THREADS - 1) fillArgs[i].end = n;
        pthread_create(&fillThreads[i], NULL, fillBucket, (void*)&fillArgs[i]);
    }

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(fillThreads[i], NULL);
    }
    pthread_t sortThreads[nbBuckets];
    SortArgs sortArgs[nbBuckets];
    for (int i = 0; i < nbBuckets; i++) {
        sortArgs[i] = (SortArgs){buckets[i], sizes[i]};
        pthread_create(&sortThreads[i], NULL, sortBucket, (void*)&sortArgs[i]);
    }

    for (int i = 0; i < nbBuckets; i++) {
        pthread_join(sortThreads[i], NULL);
    }

    int index = 0;
    for (int i = 0; i < nbBuckets; i++) {
        for (int j = 0; j < sizes[i]; j++) {
            arr[index++] = buckets[i][j];
        }
        free(buckets[i]);
    }
    free(buckets);
    free(sizes);
}

int main() {
   //int n = value;
    int* arr = (int*)malloc(n * sizeof(int));

    srand(time(NULL));
    
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % 1001;

    }


    clock_t start = clock();
    bucketSort(arr, n);
    clock_t end = clock();
    double time_taken = ((double) (end - start)) / CLOCKS_PER_SEC;

    printf("Time taken: %f seconds\n", time_taken);
    free(arr);
    return 0;
}

