#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

void insertionSort(int* bucket, int size) {
    int i, j, k;
    for (i = 1; i < size; i++) {
        k = bucket[i];
        j = i - 1;

        while (j >= 0 && bucket[j] > k) {
            bucket[j + 1] = bucket[j];
            j--;;
        }
        bucket[j + 1] = k;
    }
}

int getMin(int arr[], int n) {
    int m = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] < m) {
            m = arr[i];
        }
    }
    return m;
}

int getMax(int arr[], int n) {
    int m = arr[0];
    for (int i = 1; i < n; i++) {
        if (arr[i] > m) {
            m = arr[i];
        }
    }
    return m;
}


void bucketSort(int arr[], int n) {
	omp_set_num_threads(4);
    int max = getMax(arr, n);
    int min = getMin(arr, n);
    int range = max - min + 1;
    int nbBuckets = sqrt(n);
    int rangePerBucket = range / nbBuckets + 1;
    
    int** buckets = (int**)malloc(nbBuckets * sizeof(int*));
    int* size = (int*)calloc(nbBuckets, sizeof(int));
    
    for (int i = 0; i < nbBuckets; i++) {
        buckets[i] = (int*)malloc(n * sizeof(int));
    }


    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        int bucketIndex = (arr[i] - min) / rangePerBucket;
        #pragma omp critical
        {
            buckets[bucketIndex][size[bucketIndex]++] = arr[i];
        }
    }


    #pragma omp parallel for
    for (int i = 0; i < nbBuckets; i++) {
        if (size[i] > 0) {
            insertionSort(buckets[i], size[i]);
        }
    }

   int idx = 0;
    for (int i = 0; i < nbBuckets; i++) {
        for (int j = 0; j < size[i]; j++) {
            arr[idx++] = buckets[i][j];
        }
        free(buckets[i]); 
    }
    
	free(buckets); 
	free(size);
}

int main() {
    double start = omp_get_wtime();
  //  int n = value;

    int* arr = (int*)malloc(n*sizeof(int));

    srand(time(NULL));
    for(int i = 0; i < n; i++) {
        arr[i] = (rand() % 201) - 100;
    }

    bucketSort(arr, n);

   double duration = omp_get_wtime() - start;

    printf("Time taken %f s\n", duration);
    
    free(arr);

    return 0;
}

