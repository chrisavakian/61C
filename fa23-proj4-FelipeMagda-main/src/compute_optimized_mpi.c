#include <omp.h>
#include <x86intrin.h>
#include "compute.h"

//n = b_matrix->cols
//vec1 = position of a_matrix
//vec2 = position of b_matrix
int32_t dot(uint32_t n, int32_t *vec1, int32_t *vec2) {
            int32_t sum = 0; 
            
            __m256i vectorSum = _mm256_setzero_si256();
            __m256i a = _mm256_setzero_si256();
            __m256i b = _mm256_setzero_si256();

            //Is this correct for loop?
            //#pragma omp parallel for
            for (int s = 0; s < (n / 8 * 8); s += 8) {
              a = _mm256_loadu_si256((__m256i*)(s + vec1));
              b = _mm256_loadu_si256((__m256i*)(s + vec2));

              //#pragma omp critical
              vectorSum = _mm256_add_epi32(vectorSum, _mm256_mullo_epi32(a, b));
            }

            int sumArr[8];
            _mm256_storeu_si256((__m256i*) sumArr, vectorSum);
            for (int c = 0; c < 8; c++) {
              sum += sumArr[c];
            }

            for (int b = (n / 8 * 8); b < n; b++) {
              sum += *(vec1 + b) * *(vec2 + b);
            }
            
            return sum;
}

// Computes the convolution of two matrices
int reverseB(matrix_t *rushB) {
    //Size of b matrix
    int rushBSize = rushB->rows * rushB->cols;
    
    //temp b array to store stuff in
    //will replace rushB->data eventually
    //Don't forget to free rushB_data
    int32_t *rushB2 = malloc(rushBSize * sizeof(int32_t));
    if (rushB2 == NULL) {
        return -1;
    }
    
    //Stores the reversed rushB numbers
    #pragma omp parallel for
    for (int i = 0; i < rushBSize; i++) {
        rushB2[i] = rushB->data[rushBSize - i - 1];
    }

    //Free old data
    free(rushB->data);

    rushB->data = rushB2;

    return 0;
}

  int convolve(matrix_t *a_matrix, matrix_t *b_matrix, matrix_t **output_matrix) {
      // TODO: convolve matrix a and matrix b, and store the resulting matrix in
      // output_matrix
      
      //Init output_matrix
      (*output_matrix) = malloc(sizeof(matrix_t));
      
      int wackAsRows = a_matrix->rows - b_matrix->rows + 1;
      int wackAsCols = a_matrix->cols - b_matrix->cols + 1;

      (*output_matrix)->rows = wackAsRows;
      (*output_matrix)->cols = wackAsCols;

      (*output_matrix)->data = malloc(wackAsRows * wackAsCols * sizeof(int32_t));
      //Init output_matrix done

      //Rush B
      int err = reverseB(b_matrix);
      if (err == -1) {
          return -1;
      }
      //Rush B over
      //Note rushB is now reveresed

      //Multipto9y the matricisesd
      //#pragma omp parallel for
      int outputIndex = 0;
      #pragma omp parallel for
      for (int i = 0; i < wackAsRows; i++) {
        for (int j = 0; j < wackAsCols; j++) {
          int32_t sum = 0;
            for (int k = 0; k < b_matrix->rows; k++) {
                sum += dot(b_matrix->cols, &a_matrix->data[(i + k) * a_matrix->cols + j], &b_matrix->data[k * b_matrix->cols]);
          }
          (*output_matrix)->data[i * (*output_matrix)->cols + j] = sum;
        }
      }
    return 0;
  }

// Executes a task
int execute_task(task_t *task) {
  matrix_t *a_matrix, *b_matrix, *output_matrix;

  char *a_matrix_path = get_a_matrix_path(task);
  if (read_matrix(a_matrix_path, &a_matrix)) {
    printf("Error reading matrix from %s\n", a_matrix_path);
    return -1;
  }
  free(a_matrix_path);

  char *b_matrix_path = get_b_matrix_path(task);
  if (read_matrix(b_matrix_path, &b_matrix)) {
    printf("Error reading matrix from %s\n", b_matrix_path);
    return -1;
  }
  free(b_matrix_path);

  if (convolve(a_matrix, b_matrix, &output_matrix)) {
    printf("convolve returned a non-zero integer\n");
    return -1;
  }

  char *output_matrix_path = get_output_matrix_path(task);
  if (write_matrix(output_matrix_path, output_matrix)) {
    printf("Error writing matrix to %s\n", output_matrix_path);
    return -1;
  }
  free(output_matrix_path);

  free(a_matrix->data);
  free(b_matrix->data);
  free(output_matrix->data);
  free(a_matrix);
  free(b_matrix);
  free(output_matrix);
  return 0;
}
