#include "ex1.h"

void v_add_naive(double* x, double* y, double* z) {
    #pragma omp parallel
    {
        for(int i=0; i<ARRAY_SIZE; i++) 
        {
            z[i] = x[i] + y[i];
        }
    }
}

// Adjacent Method
void v_add_optimized_adjacent(double* x, double* y, double* z) {
    
    #pragma omp parallel
    {
        for (int i = omp_get_thread_num(); i < ARRAY_SIZE; i+= omp_get_num_threads()) {
            z[i] = x[i] + y[i];
        }
    }
}

// Chunks Method
void v_add_optimized_chunks(double* x, double* y, double* z) {
    // TODO: Implement this function
    // Do NOT use the `for` directive here!
    
    int o = 0;

    #pragma omp parallel
    {

        int num_threads = omp_get_num_threads();
        int thread_num = omp_get_thread_num();
        int cv = (ARRAY_SIZE / num_threads) * thread_num;

        for (int i = cv; i < (cv + (ARRAY_SIZE / num_threads)); i++) {
            z[i] = x[i] + y[i];
        }
            o = (ARRAY_SIZE / num_threads) * num_threads;
    } 

    
    for (int j = o; j < ARRAY_SIZE; j++) {
        z[j] = x[j] + y[j];
    }
}
