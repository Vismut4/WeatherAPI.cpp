#include <SFML/Graphics.hpp>
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>
#include <sstream>
#include <iomanip>

using namespace std;

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string roundToTwoDecimalPlaces(float value) {
    ostringstream oss;
    oss << fixed << setprecision(2) << value;
    return oss.str();
}

void fetchWeatherData(const string& city, string& temperatureToday, string& conditionToday, vector<pair<string, string>>& nextDays) {
    string apiKey = "b1d71811cfce454c87a140824242511";
    string url = "http://api.weatherapi.com/v1/forecast.json?key=" + apiKey + "&q=" + city + "&days=3";

    CURL* curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    string readBuffer;
    Json::Value root;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "Curl request failed: " << curl_easy_strerror(res) << endl;
        }
        else {
            Json::CharReaderBuilder reader;
            istringstream sstream(readBuffer);
            string errs;
            if (Json::parseFromStream(reader, sstream, &root, &errs)) {
                float tempC_today = root["current"]["temp_c"].asFloat();
                string condition_today = root["current"]["condition"]["text"].asString();
                temperatureToday = roundToTwoDecimalPlaces(tempC_today);
                conditionToday = condition_today;

                nextDays.clear();

                string date1 = root["forecast"]["forecastday"][1]["date"].asString();
                float tempC1 = root["forecast"]["forecastday"][1]["day"]["avgtemp_c"].asFloat();
                string condition1 = root["forecast"]["forecastday"][1]["day"]["condition"]["text"].asString();
                nextDays.push_back({ date1, roundToTwoDecimalPlaces(tempC1) + "°C, " + condition1 });

                string date2 = root["forecast"]["forecastday"][2]["date"].asString();
                float tempC2 = root["forecast"]["forecastday"][2]["day"]["avgtemp_c"].asFloat();
                string condition2 = root["forecast"]["forecastday"][2]["day"]["condition"]["text"].asString();
                nextDays.push_back({ date2, roundToTwoDecimalPlaces(tempC2) + "°C, " + condition2 });
            }
            else {
                cerr << "Error parsing JSON response: " << errs << endl;
            }
        }
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

int main() {
    string city;
    string temperatureToday, conditionToday;
    vector<pair<string, string>> nextDays;

    sf::RenderWindow window(sf::VideoMode(800, 800), "Weather Info");

    // Obrazek na tło
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("C:/Users/ASUS/source/repos/NewPrognozaPogody/Image/BackIMG.jpg")) {
        cerr << "Failed to load background image!" << endl;
        return 1;
    }
    sf::Sprite backgroundSprite(backgroundTexture);

    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
        cerr << "Failed to load font!" << endl;
        return 1;
    }

    sf::Text instructionText("Enter city name and press Enter:", font, 24);
    instructionText.setPosition(10, 10);

    sf::Text inputText("", font, 24);
    inputText.setPosition(10, 50);
    inputText.setFillColor(sf::Color::White);

    sf::Text weatherTodayText("", font, 24);
    weatherTodayText.setPosition(10, 150);

    sf::Text forecastText1("", font, 24);
    forecastText1.setPosition(10, 250);

    sf::Text forecastText2("", font, 24);
    forecastText2.setPosition(10, 300);

    string currentInput = "";
    bool isCityEntered = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8 && currentInput.length() > 0) { 
                    currentInput.pop_back();
                }
                else if (event.text.unicode == 13) { 
                    city = currentInput;
                    if (!city.empty()) {
                        fetchWeatherData(city, temperatureToday, conditionToday, nextDays);
                        weatherTodayText.setString("Today: " + temperatureToday + "°C, " + conditionToday);
                        if (nextDays.size() >= 2) {
                            forecastText1.setString(nextDays[0].first + ": " + nextDays[0].second);
                            forecastText2.setString(nextDays[1].first + ": " + nextDays[1].second);
                        }
                        isCityEntered = true;
                    }
                }
                else if (event.text.unicode < 128) {
                    currentInput += static_cast<char>(event.text.unicode);
                }
            }
        }

        inputText.setString(currentInput);

        window.clear();
        window.draw(backgroundSprite); 
        window.draw(instructionText);
        window.draw(inputText);

        if (isCityEntered) {
            window.draw(weatherTodayText);
            window.draw(forecastText1);
            window.draw(forecastText2);
        }

        window.display();
    }

    return 0;
}
