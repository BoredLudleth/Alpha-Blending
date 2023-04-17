#include <x86intrin.h>
#include <immintrin.h>
#include <stdio.h>

#define DUP8(a) a, a, a, a, a, a, a, a
#define DUP16(a) DUP8(a), DUP8(a)
#define DUP32(a) DUP16(a), DUP16(a)


int main () {
    __m256i _255 = _mm256_set_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
                                   17, 18 , 19, 20 , 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31);
    __m256i _mask = _mm256_set_epi8(DUP16(0x80), 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    __m256i _arr = _mm256_shuffle_epi8 (_255, _mask);
    for (int i = 0; i < 32; i++) {
        printf ("%d - %u\n", i, *(&_arr + i));
    }

    return 0;
}