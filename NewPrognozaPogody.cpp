#include <SFML/Graphics.hpp>
#include <iostream>
#include <curl/curl.h>
#include <json/json.h>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <algorithm>
#include "GradientsTexture.h" 
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

void fetchWeatherData(const string& city, string& temperatureToday, string& conditionToday, vector<pair<string, string>>& nextDays, bool& isDay) {
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

                isDay = root["current"]["is_day"].asBool();

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

void loadCityHistory(vector<string>& cityHistory) {
    ifstream historyFile("city_history.txt");
    string city;

    while (getline(historyFile, city)) {
        cityHistory.push_back(city);
    }
    historyFile.close();
}

void saveCityHistory(const vector<string>& cityHistory) {
    ofstream historyFile("city_history.txt");
    for (const auto& city : cityHistory) {
        historyFile << city << endl;
    }
    historyFile.close();
}

int main() {
    string city;
    string temperatureToday, conditionToday;
    vector<pair<string, string>> nextDays;
    bool isDay = true;
    vector<string> cityHistory;

    // Load city history
    loadCityHistory(cityHistory);

    sf::RenderWindow window(sf::VideoMode(800, 600), "Weather Info");

    // Using the gradient functions
    sf::Texture dayTexture = createDayGradient();
    sf::Texture nightTexture = createNightGradient();
    sf::Texture noCityTexture = createNoCityGradient();

    sf::Sprite backgroundSprite(noCityTexture); // Default to no city texture

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        cerr << "Failed to load font!" << endl;
        return 1;
    }

    // Texts and buttons
    sf::Text inputText("", font, 24);
    inputText.setPosition(10, 50);
    inputText.setFillColor(sf::Color::Black);

    sf::RectangleShape inputBox(sf::Vector2f(300, 40));
    inputBox.setPosition(10, 50);
    inputBox.setFillColor(sf::Color::White);
    inputBox.setOutlineColor(sf::Color::Black);
    inputBox.setOutlineThickness(1);

    sf::RectangleShape button(sf::Vector2f(100, 40));
    button.setPosition(320, 50);
    button.setFillColor(sf::Color(100, 149, 237));

    sf::Text buttonText("Szukac", font, 24);
    buttonText.setPosition(330, 55);
    buttonText.setFillColor(sf::Color::White);

    sf::RectangleShape historyButton(sf::Vector2f(100, 40));
    historyButton.setPosition(440, 50);
    historyButton.setFillColor(sf::Color(100, 149, 237));

    sf::Text historyButtonText("History", font, 24);
    historyButtonText.setPosition(450, 55);
    historyButtonText.setFillColor(sf::Color::White);

    sf::RectangleShape newWindowButton(sf::Vector2f(150, 40));
    newWindowButton.setPosition(320, 100);
    newWindowButton.setFillColor(sf::Color(100, 149, 237));

    sf::Text newWindowButtonText("Wykres", font, 24);
    newWindowButtonText.setPosition(330, 105);
    newWindowButtonText.setFillColor(sf::Color::White);

    sf::Text weatherTodayText("", font, 24);
    weatherTodayText.setPosition(10, 150);

    sf::Text forecastText1("", font, 24);
    forecastText1.setPosition(10, 250);

    sf::Text forecastText2("", font, 24);
    forecastText2.setPosition(10, 300);

    // Text to show city history
    sf::Text historyText("", font, 18);
    historyText.setPosition(560, 50);
    historyText.setFillColor(sf::Color::Black);
    historyText.setOutlineColor(sf::Color::White);
    historyText.setOutlineThickness(1);

    string currentInput = "";
    bool isCityEntered = false;
    bool isHistoryVisible = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode == 8 && !currentInput.empty()) { // Backspace
                    currentInput.pop_back();
                }
                else if (event.text.unicode < 128 && event.text.unicode != 13) { // Add char
                    currentInput += static_cast<char>(event.text.unicode);
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                    // Click "Szukac" button
                    if (button.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        city = currentInput;
                        if (!city.empty()) {
                            fetchWeatherData(city, temperatureToday, conditionToday, nextDays, isDay);

                            if (find(cityHistory.begin(), cityHistory.end(), city) == cityHistory.end()) {
                                if (cityHistory.size() >= 3) {
                                    cityHistory.erase(cityHistory.begin());
                                }
                                cityHistory.push_back(city);
                                saveCityHistory(cityHistory);
                            }

                            if (isDay)
                                backgroundSprite.setTexture(dayTexture);
                            else
                                backgroundSprite.setTexture(nightTexture);

                            weatherTodayText.setString("Today: " + temperatureToday + "°C, " + conditionToday);
                            if (nextDays.size() >= 2) {
                                forecastText1.setString(nextDays[0].first + ": " + nextDays[0].second);
                                forecastText2.setString(nextDays[1].first + ": " + nextDays[1].second);
                            }
                            isCityEntered = true;
                        }
                        else {
                            backgroundSprite.setTexture(noCityTexture);  // Green gradient if city is not entered
                            isCityEntered = false;
                        }
                    }
                    else if (historyButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        isHistoryVisible = !isHistoryVisible;
                    }
                    // Open new window
                    else if (newWindowButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
                        sf::RenderWindow newWindow(sf::VideoMode(800, 600), "New Window");
                        while (newWindow.isOpen()) {
                            sf::Event newEvent;
                            while (newWindow.pollEvent(newEvent)) {
                                if (newEvent.type == sf::Event::Closed)
                                    newWindow.close();
                            }
                            newWindow.clear(sf::Color::White);
                            newWindow.display();
                        }
                    }
                    else if (isHistoryVisible) {
                        for (int i = 0; i < cityHistory.size(); ++i) {
                            sf::FloatRect cityRect(560, 50 + i * 30, 300, 20);
                            if (cityRect.contains(mousePos.x, mousePos.y)) {
                                currentInput = cityHistory[i];
                                isHistoryVisible = false;
                            }
                        }
                    }
                }
            }
        }

        string suggestions = "";
        if (isHistoryVisible) {
            for (int i = 0; i < cityHistory.size(); ++i) {
                suggestions += cityHistory[i] + "\n";
            }
            historyText.setString(suggestions);
        }
        else {
            historyText.setString("");
        }

        inputText.setString(currentInput);

        window.clear();
        window.draw(backgroundSprite);
        window.draw(inputBox);
        window.draw(inputText);
        window.draw(button);
        window.draw(buttonText);
        window.draw(historyButton);
        window.draw(historyButtonText);
        window.draw(newWindowButton);
        window.draw(newWindowButtonText);
        window.draw(historyText);

        if (isCityEntered) {
            window.draw(weatherTodayText);
            window.draw(forecastText1);
            window.draw(forecastText2);
        }

        window.display();
    }

    return 0;
}
