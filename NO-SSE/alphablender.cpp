#include "alphablender.hpp"

int lenFile(FILE *text) {
    fseek(text, 0, SEEK_END);
    int length =  ftell(text);
    fseek(text, 0, SEEK_SET);

    return length;    
}

void blendPix (unsigned char* pixels, unsigned char* kit_pix, int x0, int y0, sf::Vector2i sizeBack, sf::Vector2i sizeKit) {
    unsigned int pixel_red      = 0;
    unsigned int pixel_green    = 0;
    unsigned int pixel_blue     = 0;
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

            pixel_red      += kit_pix[back_pixel]     * kit_pix[back_pixel + 3];
            pixel_green    += kit_pix[back_pixel + 1] * kit_pix[back_pixel + 3];
            pixel_blue     += kit_pix[back_pixel + 2] * kit_pix[back_pixel + 3];
            pixel_reserved += kit_pix[back_pixel + 3] * kit_pix[back_pixel + 3];

            pixel_red      = pixel_red >> 8;
            pixel_green    = pixel_green >> 8;
            pixel_blue     = pixel_blue >> 8;
            pixel_reserved = pixel_reserved >> 8;

            pixels[front_pixel] = (unsigned char) pixel_red;
            pixels[front_pixel + 1] = (unsigned char) pixel_green;
            pixels[front_pixel + 2] = (unsigned char) pixel_blue;
            pixels[front_pixel + 3] = (unsigned char) pixel_reserved;
        }
    }
}