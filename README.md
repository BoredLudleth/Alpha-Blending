# Оптимизация Alpha Blending с помощью SIMD
### Дисклеймер ###
Всем привет,
этот второй проект по оптимизации некоторых программ с помощью SIMD. Подробнее про SIMD-оптимизацию я написал в прошлой работе [множество Мондельброта](https://github.com/BoredLudleth/Mondelbrot-Set.git). 
Данный проект написан как учебное задание для курса И.Р.Дединского, с целью разобраться в SIMD (Single Instruction Multiple Data).
Цель проекта - ускорение вычислений за счет параллельной обработки данных на уровне аппаратных команд.

Для визуализации картинки использовалась библиотека SFML.

Для ускорения программы я пользовался SSE2.

#### Характеристики моего ноутбука ####
OS: Ubuntu 22.04.1 LTS Jellyfish

Процессор: AMD Ryzen 5 4000 series

gcc version 11.3.0

## Компиляция ##
```
make link    - линковка проекта

make compile - компиляция проекта

make         - компиляция + линковка

./main       - для запуска картинки, в терминале будет текущий FPS 
```

## Alpha Blending ##
В первую очередь следует рассказать о том что такое alpha blending и что мы будем оптимизировать.

В компьютерной графике альфа-композитинг или альфа-смешение — это процесс объединения одного изображения с фоном для создания видимости частичной 
или полной прозрачности. То есть простыми словами это наложение одной картинки на другую.

Каждая картинка состоит из маленьких ячеек, называемых  пикселями. В каждом  пикселе есть информация о цвете данной ячейки. Существует цветовая модель RGBA, которая 
расшифровывается в red greeen blue alpha. Эта модель является более расширенной по сравнению с RGB, поскольку там есть дополнительный бит отвечающий за прозрачность, 
а остальные три аргумента отвечают за сам цвет.

Рассмотрим формулу по которой мы будем пересчитывать пиксели в итоговом изображение на примере смешения двух пикселей.

>$finalColor = frontColor * frontAlpha + backColor * (1 - frontAlpha)$

Следует немного пояснить формулу: 

front - цвет картинки переднего плана ( эту картинку мы накладываем на back), 

frontAlpha - его прозрачность в данном пикселе которая может  иметь значение [0; 1]
0 будет соответствовать полной прозрачности изображения на переднем плане, а 1 соответствует абсолютно непрозрачному цвету,

backColor - цвет картинки заднего плана ( на которую мы накладываем front ).

Данная формула примет некоторые изменения, поскольку каждый цвет выражается 3 цветами и прозрачность принимает значение [0; 255]


## Реализация без SIMD ##
Ниже представлен кусочек кода, который вычисляет массив пикселей итогового изображения.

```C
void blendPix (unsigned char* pixels, unsigned char* kit_pix, int x0, int y0, sf::Vector2i sizeBack, sf::Vector2i sizeKit) {  //pixels массив со значениями пикселей background
    unsigned int pixel_red      = 0;                                                                                          //kit_pix массив со значениями пикселей foreground
    unsigned int pixel_green    = 0;                                                                                          //x0 и y0 место куда мы хотим поместить верхний левый угол foreground
    unsigned int pixel_blue     = 0;                                                                                          //sizeBack и sizeKit - размеры двух изображений по осям X и Y backgroundа и foregroundа соответственно
    unsigned int pixel_reserved = 0;

    int front_pixel = 0;
    int back_pixel  = 0;

    for (int y = 0; y < sizeKit.y; y++) {
        for (int x = 0; x < sizeKit.x; x++) {
            front_pixel = 4 * ((y0 + y) * sizeBack.x + x0 + x);        
            back_pixel  = 4 * (y * sizeKit.x + x);

            pixel_red      = pixels[front_pixel]     * (255 - kit_pix[back_pixel+ 3]);
            pixel_green    = pixels[front_pixel + 1] * (255 - kit_pix[back_pixel + 3]);
            pixel_blue     = pixels[front_pixel + 2] * (255 - kit_pix[back_pixel + 3]);
            pixel_reserved = pixels[front_pixel + 3] * (255 - kit_pix[back_pixel+ 3]);

            pixel_red      += kit_pix[back_pixel]     * kit_pix[back_pixel + 3];                                              // в этом месте мы накладываем пиксели друг на друга
            pixel_green    += kit_pix[back_pixel + 1] * kit_pix[back_pixel + 3];
            pixel_blue     += kit_pix[back_pixel + 2] * kit_pix[back_pixel + 3];
            pixel_reserved += kit_pix[back_pixel + 3] * kit_pix[back_pixel + 3];

            pixel_red      = pixel_red >> 8;                                                                                  // но мы можем заметить, что пока что это не цвета, поскольку даже каждое значение может быть больше чем может содержать 1 char
            pixel_green    = pixel_green >> 8;                                                                                // поэтому с помощью логического сдвига поделим каждое значение на 256, поскольку в нашей формуле значение прозрачности находится [0; 1]
            pixel_blue     = pixel_blue >> 8;                                                                                 // следовало разделить на 255, но гораздо лучше будет сделать логический сдвиг, так как на аппаратном уровне он будет быстрее и
            pixel_reserved = pixel_reserved >> 8;                                                                             // полученная погрешность не будет заметна для глаза

            pixels[front_pixel] = (unsigned char) pixel_red;
            pixels[front_pixel + 1] = (unsigned char) pixel_green;
            pixels[front_pixel + 2] = (unsigned char) pixel_blue;
            pixels[front_pixel + 3] = (unsigned char) pixel_reserved;                                                          // перезаписываем пиксели
        }
    }
}
    
```

Исследуем как ключи оптимизации повлияют на время исполнения программы:

| Ключ оптимизации |        time, sec        |
|------------------|-------------------------|
|       None       |         0.001777        |
|       -O1        |         0.001780        |
|       -O2        |         0.001792        |
|       -O3        |         0.001872        |
|       -Ofast     |         0.002022        |

Заметим, что в данном случае использование ключей оптимизации не только не оказывает эффекта ускорения программы, а даже наоборот программа стала немного медленней.


## Реализация с SSE2 ##
```C
void blendPix (unsigned char* pixels, unsigned char* kit_pix, int x0, int y0, sf::Vector2i sizeBack, sf::Vector2i sizeKit) {
    __m128i _fr = _mm_set_epi8 (DUP16(0));        //little
    __m128i _FR = _mm_set_epi8 (DUP16(0));
    __m128i _bk = _mm_set_epi8 (DUP16(0));        //old
    __m128i _BK = _mm_set_epi8 (DUP16(0));

    __m128i _moveA = _mm_set_epi8 (MEOW, MEOW, MEOW, MEOW, MEOW, MEOW, MEOW, MEOW, 14, 14, 14, 14, 6, 6, 6, 6);

    // __m128i _255 = _mm_set_epi16(DUP8(255u)); 
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
    
```


| Ключ оптимизации |        time, sec        |
|------------------|-------------------------|
|       None       |         0.001777        |
|       -O1        |         0.002039        |
|       -O2        |         0.001820        |
|       -O3        |         0.001253        |
|       -Ofast     |         0.001204        |

## Вывод ##

В работе с большими массивами флаги компиляции могут быть не всегда эффективными, поэтому чтобы сделать программу ещё быстрей следует пользоваться SIMD инструкциями.

![image](https://github.com/BoredLudleth/Alpha-Blending/assets/111277087/74b92883-2417-4a5b-ad70-caaf6da2d9a9)












