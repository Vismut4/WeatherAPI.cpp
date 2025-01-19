#ifndef GRADIENTS_H
#define GRADIENTS_H

#include <SFML/Graphics.hpp>

sf::Texture createDayGradient() {
    sf::Image dayImage;
    dayImage.create(800, 600);
    for (unsigned int y = 0; y < 600; ++y) {
        sf::Uint8 r = 255;
        sf::Uint8 g = 255 - (y * 130 / 600);  // From yellow to red
        sf::Uint8 b = 0 + (y * 255 / 600);    // From yellow to orange
        for (unsigned int x = 0; x < 800; ++x) {
            dayImage.setPixel(x, y, sf::Color(r, g, b));
        }
    }

    sf::Texture dayTexture;
    dayTexture.loadFromImage(dayImage);
    return dayTexture;
}

sf::Texture createNightGradient() {
    sf::Image nightImage;
    nightImage.create(800, 600);
    for (unsigned int y = 0; y < 600; ++y) {
        sf::Uint8 r = 25 + (y * 30 / 600);   // From dark blue to light blue
        sf::Uint8 g = 25 + (y * 0 / 600);    // Constant blue
        sf::Uint8 b = 112 - (y * 112 / 600); // From blue to purple
        for (unsigned int x = 0; x < 800; ++x) {
            nightImage.setPixel(x, y, sf::Color(r, g, b));
        }
    }

    sf::Texture nightTexture;
    nightTexture.loadFromImage(nightImage);
    return nightTexture;
}

sf::Texture createNoCityGradient() {
    sf::Image noCityImage;
    noCityImage.create(800, 600);
    for (unsigned int y = 0; y < 600; ++y) {
        sf::Uint8 r = 0;
        sf::Uint8 g = 255 - (y * 255 / 600);  // From dark green to light green
        sf::Uint8 b = 0;                      // Constant green
        for (unsigned int x = 0; x < 800; ++x) {
            noCityImage.setPixel(x, y, sf::Color(r, g, b));
        }
    }

    sf::Texture noCityTexture;
    noCityTexture.loadFromImage(noCityImage);
    return noCityTexture;
}

#endif

