#include <stdio.h>

int main() {
  // Assign x (an integer) to 5
  int x = 5;
  int* pointer_to_x;
  // TODO: create a pointer to x
  // Hint: the first blank should be a variable type
  //       the second blank should be the address of x
  pointer_to_x = &x;

  // This line should print 5
  printf("%d\n", *pointer_to_x);

  return 0;
}
