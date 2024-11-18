#include <SFML/Graphics.hpp>
#include <curl/curl.h>
#include <json/json.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "WeatherImages.h"

using namespace std;

size_t callback(const char* in, size_t size, size_t num, string* out) {
    const size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

int main() {
    string apiKey = "2ec0ddea917f8211e0fb0b8919c86228";
    string city;
    bool weatherFetched = false;

    sf::RenderWindow window(sf::VideoMode(1080, 640), "Prognoza Pogody");

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        cerr << "Blad ladowania czcionki!" << endl;
        return 1;
    }

    sf::Text inputText("Podaj miasto: ", font, 24);
    inputText.setPosition(20, 20);
    inputText.setFillColor(sf::Color::White);

    sf::Text cityInput("", font, 24);
    cityInput.setPosition(20, 60);
    cityInput.setFillColor(sf::Color::White);

    sf::Text weatherText("", font, 24);
    weatherText.setPosition(20, 120);
    weatherText.setFillColor(sf::Color::White);

    sf::Text tempText("", font, 24);
    tempText.setPosition(20, 160);
    tempText.setFillColor(sf::Color::White);

    sf::Text humidityText("", font, 24);
    humidityText.setPosition(20, 200);
    humidityText.setFillColor(sf::Color::White);

    sf::RectangleShape blackSquare(sf::Vector2f(300.f, 250.f));
    blackSquare.setFillColor(sf::Color::Black);
    blackSquare.setPosition(10, 10);

    sf::Texture weatherTexture;
    sf::Sprite weatherSprite;

    weatherTexture.loadFromFile("C:/Users/ASUS/source/repos/Project_Prognoza_Pogody/Project_Prognoza_Pogody/Weatherjpg/default.jpg");
    weatherSprite.setTexture(weatherTexture);

    WeatherImages weatherImages;

    sf::RectangleShape restartButton(sf::Vector2f(120.f, 50.f));
    restartButton.setFillColor(sf::Color::Magenta);
    restartButton.setPosition(920, 570);

    sf::Text restartText("Restart", font, 24);
    restartText.setFillColor(sf::Color::White);
    restartText.setPosition(930, 580);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::TextEntered && !weatherFetched) {
                if (event.text.unicode == '\b') {
                    if (!city.empty()) {
                        city.pop_back();
                    }
                }
                else if (event.text.unicode < 128) {
                    city += static_cast<char>(event.text.unicode);
                }
                cityInput.setString(city);
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Return && !city.empty() && !weatherFetched) {
                string url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";

                CURL* curl = curl_easy_init();
                string weather;
                double temp;
                int humidity;

                if (curl) {
                    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
                    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

                    string response;
                    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
                    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

                    CURLcode res = curl_easy_perform(curl);

                    if (res != CURLE_OK) {
                        cerr << "Blad: " << curl_easy_strerror(res) << endl;
                        weatherText.setString("Blad: Nie udalo sie pobrac pogody.");
                    }
                    else {
                        Json::CharReaderBuilder builder;
                        Json::CharReader* reader = builder.newCharReader();
                        Json::Value jsonData;
                        string errs;

                        if (reader->parse(response.c_str(), response.c_str() + response.length(), &jsonData, &errs)) {
                            weather = jsonData["weather"][0]["description"].asString();
                            temp = jsonData["main"]["temp"].asDouble();
                            humidity = jsonData["main"]["humidity"].asInt();

                            weatherText.setString("Pogoda: " + weather);
                            stringstream stream;
                            stream << fixed << std::setprecision(2) << temp;
                            tempText.setString("Temperatura: " + stream.str() + " °C");
                            humidityText.setString("Wilgotnosc: " + to_string(humidity) + " %");

                            string imagePath = weatherImages.getImagePath(weather);
                            weatherTexture.loadFromFile(imagePath);
                            weatherSprite.setTexture(weatherTexture);
                        }
                        else {
                            cerr << "Blad podczas parsowania JSON: " << errs << endl;
                        }

                        delete reader;
                    }
                    curl_easy_cleanup(curl);
                }
                weatherFetched = true;
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (restartButton.getGlobalBounds().contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        city.clear();
                        cityInput.setString("");
                        weatherText.setString("");
                        tempText.setString("");
                        humidityText.setString("");
                        weatherFetched = false;

                        weatherTexture.loadFromFile("C:/Users/ASUS/source/repos/Project_Prognoza_Pogody/Project_Prognoza_Pogody/Weatherjpg/default.jpg");
                        weatherSprite.setTexture(weatherTexture);
                    }
                }
            }
        }

        window.clear();
        window.draw(weatherSprite);
        window.draw(blackSquare);
        window.draw(inputText);
        window.draw(cityInput);
        if (weatherFetched) {
            window.draw(weatherText);
            window.draw(tempText);
            window.draw(humidityText);
        }
        window.draw(restartButton);
        window.draw(restartText);
        window.display();
    }

    return 0;
}
