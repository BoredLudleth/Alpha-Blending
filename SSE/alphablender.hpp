#ifndef ALPHABLENDER_HPP
#define ALPHABLENDER_HPP

#include <SFML/Graphics.hpp>
#include <string.h>
#include <time.h>

#include <x86intrin.h>
#include <immintrin.h>

#define DUP8(a) a, a, a, a, a, a, a, a
#define DUP16(a) DUP8(a), DUP8(a)

#define MEOW 0x80
#define Z 0


int lenFile(FILE *text);            //ОНО МНЕ НАДО?

void blendPix (unsigned char* pixels, unsigned char* kit_pix, int x0, int y0, sf::Vector2i sizeBack, sf::Vector2i sizeKit);

inline __m256i _mm256_loadmove_epi8 (char* src);

#endif