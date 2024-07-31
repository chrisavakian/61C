#include "compute.h"
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
    for (int i = 0; i < wackAsRows; i++) {
        for (int j = 0; j < wackAsCols; j++) {
            int sum = 0;
            for (int k = 0; k < b_matrix->rows; k++) {
                for (int l = 0; l < b_matrix->cols; l++) {
                    sum += a_matrix->data[(i + k) * a_matrix->cols + j + l] * b_matrix->data[k * b_matrix->cols + l];
                }
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
