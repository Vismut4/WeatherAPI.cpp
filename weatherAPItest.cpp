#include <SFML/Graphics.hpp>
#include <curl/curl.h>
#include <json/json.h>
#include <string>
#include <iostream>
#include <iomanip>  /
#include <sstream> 

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

    sf::RenderWindow window(sf::VideoMode(1080, 640), "Prognoz Pogody");

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
                            weatherFetched = true; 
                        }
                        else {
                            cerr << "Błąd parsowania JSON: " << errs << endl;
                            weatherText.setString("Blad parsowania JSON");
                        }

                        delete reader;
                    }

                    curl_easy_cleanup(curl);
                }
            }
        }

        window.clear();
        window.draw(inputText);
        window.draw(cityInput);
        if (weatherFetched) {
            window.draw(weatherText);
            window.draw(tempText);
            window.draw(humidityText);
        }
        window.display();
    }

    return 0;
}
