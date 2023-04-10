#ifndef ALPHABLENDER_HPP
#define ALPHABLENDER_HPP

#include <SFML/Graphics.hpp>
#include <string.h>
#include <time.h>


int lenFile(FILE *text);            //ОНО МНЕ НАДО?

void blendPix (unsigned char* pixels, unsigned char* kit_pix, int x0, int y0, sf::Vector2i sizeBack, sf::Vector2i sizeKit);

#endif