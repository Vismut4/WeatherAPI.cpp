#include <iostream>
#include <string>
#include <curl/curl.h>
#include <json/json.h>

using namespace std;

size_t callback(const char* in, size_t size, size_t num, string* out) {
    const size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

int main() {
    string apiKey = "2ec0ddea917f8211e0fb0b8919c86228";
    string city;

    cout << "Podaj nazwe miasta: ";
    getline(cin, city);

    string url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";

    CURL* curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            cerr << "Blad: " << curl_easy_strerror(res) << endl;
        }
        else {
            cout << "Odpowiedz serwera: " << response << endl;

            Json::CharReaderBuilder builder;
            Json::CharReader* reader = builder.newCharReader();
            Json::Value jsonData;
            string errs;

            if (reader->parse(response.c_str(), response.c_str() + response.length(), &jsonData, &errs)) {
                string weather = jsonData["weather"][0]["description"].asString();
                double temp = jsonData["main"]["temp"].asDouble();
                int humidity = jsonData["main"]["humidity"].asInt();

                cout << "Miasto: " << city << endl;
                cout << "Pogoda: " << weather << endl;
                cout << "Temperatura: " << temp << "°C" << endl;
                cout << "Wilgotnosc: " << humidity << "%" << endl;
            }
            else {
                cerr << "Błąd parsowania JSON: " << errs << endl;
            }

            delete reader;
        }

        curl_easy_cleanup(curl);
    }
    else {
        cerr << "Blad inicjalizacji curl!" << endl;
    }

    return 0;
}
