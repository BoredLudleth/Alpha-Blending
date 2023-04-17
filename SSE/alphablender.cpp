#include "alphablender.hpp"

void blendPix (unsigned char* pixels, unsigned char* kit_pix, int x0, int y0, sf::Vector2i sizeBack, sf::Vector2i sizeKit) {
    __m128i _fr = _mm_set_epi8 (DUP16(0));        //little
    __m128i _FR = _mm_set_epi8 (DUP16(0));
    __m128i _bk = _mm_set_epi8 (DUP16(0));        //old
    __m128i _BK = _mm_set_epi8 (DUP16(0));

    __m128i _moveA = _mm_set_epi8 (MEOW, MEOW, MEOW, MEOW, MEOW, MEOW, MEOW, MEOW, 14, 14, 14, 14, 6, 6, 6, 6);

    __m128i _255 = _mm_set_epi16(DUP8(255u)); 
    __m128i _moveOldPix = _mm_set_epi8 (MEOW, MEOW, MEOW, MEOW, MEOW, MEOW, MEOW, MEOW, 15, 13, 11, 9, 7, 5, 3, 1);

    __m128i _alpha = _mm_set1_epi8 (0);
    __m128i _ALPHA = _mm_set1_epi8 (0);

//==========================================================================================================================
    //examples would be with __m64
    //little example (convert 8bit to 16bit) 
    //as i guess it make from 255 255 255 255 255 255 255 255 -> 0 255 0 255 0 255 0 255
    //such as 0 0 0 0 255 255 255 255 -> 0 255 0 255 0 255 0 255
//==========================================================================================================================

//==========================================================================================================================
    //we want to get from 7 6 5 4 3 2 1 0 -> 0 0 0 0 7 6 5 4
    //first idea use shuffle -> meow meow meow meow 7 6 5 4
    //second idea use move _mm_movehl_ps to take 4 most old bytes
//==========================================================================================================================
    __m128i a = _mm_set_epi8 (DUP16(0));
    __m128i A = _mm_set_epi8 (DUP16(0));

    __m128i _res = _mm_set_epi8 (DUP16(0));
    __m128i _RES = _mm_set_epi8 (DUP16(0));

    int front_pixel = 0;
    int back_pixel  = 0;

    for (int y = 0; y < sizeKit.y; y += 1) {
        for (int x = 0; x < sizeKit.x; x += 4) {
            back_pixel = 4 * ((y0 + y) * sizeBack.x + x0 + x);
            front_pixel  = 4 * (y * sizeKit.x + x);

            _fr = _mm_load_si128 ((__m128i*) &(pixels[back_pixel]));
            _bk = _mm_load_si128 ((__m128i*) &(kit_pix[front_pixel]));

            _FR = (__m128i) _mm_movehl_ps ((__m128) _fr, (__m128) _fr);
            _BK = (__m128i) _mm_movehl_ps ((__m128) _bk, (__m128) _bk);

            _fr = _mm_cvtepu8_epi16 (_fr);
            _bk = _mm_cvtepu8_epi16 (_bk);
            

            _FR = _mm_cvtepu8_epi16 (_FR);
            _BK = _mm_cvtepu8_epi16 (_BK);

            a = _mm_cvtepu8_epi16(_mm_shuffle_epi8 (_bk, _moveA));
            A = _mm_cvtepu8_epi16(_mm_shuffle_epi8 (_BK, _moveA));

            _alpha = _mm_sub_epi16 (_255, a);
            _ALPHA = _mm_sub_epi16 (_255, A);

            _res = _mm_add_epi16 (_mm_mullo_epi16 (_fr, _alpha), _mm_mullo_epi16 (_bk, a));
            _RES = _mm_add_epi16 (_mm_mullo_epi16 (_FR, _ALPHA), _mm_mullo_epi16 (_BK, A));

            _res = _mm_shuffle_epi8 (_res, _moveOldPix);
            _RES = _mm_shuffle_epi8 (_RES, _moveOldPix);

            __m128i color = (__m128i) _mm_movelh_ps ((__m128) _res, (__m128) _RES);   

            _mm_store_si128 ((__m128i *) (pixels + back_pixel), color);
        }
    }
}