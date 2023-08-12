#include "alphablender.hpp"

int main () {

    clock_t begin = 0;
    clock_t end = 0;

    sf::Image background;
    if (!background.loadFromFile("Table.bmp")) {
        printf ("Error: image Table.bmp didn't opened\n");
        return -1;
    }


    sf::Image Cat;
    if (!Cat.loadFromFile("AskhatCat.bmp")) {
        printf ("Error: image AskhatCat.bmp didn't opened\n");
        return -1;
    }
    sf::Image foreground;
    sf::Color Invisible(0, 0, 0, 0);
    sf::IntRect newIm (0, 0, 236, 126);             //make constants
    foreground.create (240, 126, Invisible);        //make constants
    foreground.copy (Cat, 0, 0, newIm);

    const int WIDTH =  background.getSize().x;
    const int HEIGHT  = background.getSize().y;
    sf::Vector2 sizeBack(WIDTH, HEIGHT);

    const int KIT_WIDTH  = foreground.getSize().x;
    const int KIT_HEIGHT  = foreground.getSize().y;
    sf::Vector2 sizeKit(KIT_WIDTH, KIT_HEIGHT);


    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT, true), "Alpha-Volki");

    unsigned char pixels[4 * WIDTH * HEIGHT] = {};
    unsigned char kit_pix[4 * KIT_WIDTH * KIT_HEIGHT] = {};

    // begin = clock();

    memcpy (pixels, background.getPixelsPtr(), 4 * WIDTH * HEIGHT);
    memcpy (kit_pix, foreground.getPixelsPtr(), 4 * KIT_WIDTH * KIT_HEIGHT);

    sf::Texture Frame;
    if (!Frame.loadFromFile("Table.bmp")) 
    {
        printf ("Error: problem with loading texture\n");
        return -1;
    }

    sf::Sprite sprite(Frame);
    sprite.setPosition(0,0);

    //while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                break;
            }

            begin = clock();
            memcpy (pixels, background.getPixelsPtr(), 4 * WIDTH * HEIGHT);

            blendPix (pixels, kit_pix, 300, 300, sizeBack, sizeKit); 
            
            Frame.update(pixels, WIDTH,HEIGHT, 0, 0);
            //window.clear();
            window.draw(sprite);
            window.display();
            end = clock();

            //system ("clear");
        }
        
        // end = clock();

        printf ("FPS: %lf\n", (double) (end - begin) / CLOCKS_PER_SEC);
    //}

    return 0;
}