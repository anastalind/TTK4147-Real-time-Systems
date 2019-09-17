#include "array.c"

void main() {
    Array x15_array = array_new(10);
    Array x2_array = array_new(10);


    for (int j = 0; j < 2000; j++) {
      array_insertBack(&x15_array, j, 1.5);
      printf("1.5 array: %p\n", x15_array.data);

    }

    for (int k = 0; k < 2000; k++) {
      array_insertBack(&x2_array, k, 2);
      printf("2 array: %p\n", &x2_array.data[x2_array.back]);

    }


}
