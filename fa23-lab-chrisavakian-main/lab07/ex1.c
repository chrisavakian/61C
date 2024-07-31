#include <time.h>
#include <stdio.h>
#include <x86intrin.h>
#include "ex1.h"

long long int sum(int vals[NUM_ELEMS]) {
    clock_t start = clock();

    long long int sum = 0;
    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        for(unsigned int i = 0; i < NUM_ELEMS; i++) {
            if(vals[i] >= 128) {
                sum += vals[i];
            }
        }
    }
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return sum;
}

long long int sum_unrolled(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    long long int sum = 0;

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        for(unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
            if(vals[i] >= 128) sum += vals[i];
            if(vals[i + 1] >= 128) sum += vals[i + 1];
            if(vals[i + 2] >= 128) sum += vals[i + 2];
            if(vals[i + 3] >= 128) sum += vals[i + 3];
        }

        // TAIL CASE, for when NUM_ELEMS isn't a multiple of 4
        // NUM_ELEMS / 4 * 4 is the largest multiple of 4 less than NUM_ELEMS
        // Order is important, since (NUM_ELEMS / 4) effectively rounds down first
        for(unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++) {
            if (vals[i] >= 128) {
                sum += vals[i];
            }
        }
    }
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return sum;
}

long long int sum_simd(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    __m128i _127 = _mm_set1_epi32(127); // This is a vector with 127s in it... Why might you need this?
    long long int result = 0; // This is where you should put your final result!
    /* DO NOT MODIFY ANYTHING ABOVE THIS LINE (in this function) */

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        /* YOUR CODE GOES HERE */   
        for (unsigned int z = 0; z < ((NUM_ELEMS / 4) * 4); z+=4) {
            __m128i load_first =  _mm_loadu_si128(vals + z);

            //Some sort of comparison with _127
            __m128i temp = _mm_cmpgt_epi32(load_first, _127);
            load_first = _mm_and_si128(temp, load_first);
        
            int tmp_arr[4];
            _mm_storeu_si128((__m128i *) tmp_arr, load_first);
            result += tmp_arr[0] + tmp_arr[1] + tmp_arr[2] + tmp_arr[3]; 
        }        
        for (int zx = ((NUM_ELEMS / 4) * 4); zx < NUM_ELEMS; zx++ ) {
            if (vals[zx] > 127) {
                result += vals[zx];
            }
        }
    }
    /* DO NOT MODIFY ANYTHING BELOW THIS LINE (in this function) */
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return result;
}

long long int sum_simd_unrolled(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    __m128i _127 = _mm_set1_epi32(127);
    long long int result = 0;
    /* DO NOT MODIFY ANYTHING ABOVE THIS LINE (in this function) */

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        /* YOUR CODE GOES HERE */
        /* Copy your sum_simd() implementation here, and unroll it */

        for (unsigned int z = 0; z < ((NUM_ELEMS / 16) * 16); z+=16) {
            __m128i load_first =  _mm_loadu_si128(vals + z);

            //Some sort of comparison with _127
            __m128i temp1 = _mm_cmpgt_epi32(load_first, _127);
            load_first = _mm_and_si128(temp1, load_first);
        
            int tmp_arr1[4];
            _mm_storeu_si128((__m128i *) tmp_arr1, load_first);
            result += tmp_arr1[0] + tmp_arr1[1] + tmp_arr1[2] + tmp_arr1[3];



            __m128i load_second =  _mm_loadu_si128(vals + z + 4);

            //Some sort of comparison with _127
            __m128i temp2 = _mm_cmpgt_epi32(load_second, _127);
            load_second = _mm_and_si128(temp2, load_second);
        
            int tmp_arr2[4];
            _mm_storeu_si128((__m128i *) tmp_arr2, load_second);
            result += tmp_arr2[0] + tmp_arr2[1] + tmp_arr2[2] + tmp_arr2[3];




            __m128i load_third =  _mm_loadu_si128(vals + z + 8);

            //Some sort of comparison with _127
            __m128i temp3 = _mm_cmpgt_epi32(load_third, _127);
            load_third = _mm_and_si128(temp3, load_third);
        
            int tmp_arr3[4];
            _mm_storeu_si128((__m128i *) tmp_arr3, load_third);
            result += tmp_arr3[0] + tmp_arr3[1] + tmp_arr3[2] + tmp_arr3[3];



            __m128i load_fourth =  _mm_loadu_si128(vals + z + 12);

            //Some sort of comparison with _127
            __m128i temp4 = _mm_cmpgt_epi32(load_fourth, _127);
            load_fourth = _mm_and_si128(temp4, load_fourth);
        
            int tmp_arr4[4];
            _mm_storeu_si128((__m128i *) tmp_arr4, load_fourth);
            result += tmp_arr4[0] + tmp_arr4[1] + tmp_arr4[2] + tmp_arr4[3];

        }      
        
        for (int zx = ((NUM_ELEMS / 16) * 16); zx < NUM_ELEMS; zx++ ) {
            if (vals[zx] > 127) {
                result += vals[zx];
            }
        }
               /* Hint: you'll need 1 or maybe 2 tail cases here. */
    }

    /* DO NOT MODIFY ANYTHING BELOW THIS LINE (in this function) */
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return result;
}
