#ifndef WEATHER_IMAGES_H
#define WEATHER_IMAGES_H

#include <unordered_map>
#include <string>

using namespace std;

class WeatherImages {
public:
    WeatherImages() {
        const string WayToJPG = "C:/Users/ASUS/source/repos/Project_Prognoza_Pogody/Project_Prognoza_Pogody/Weatherjpg/";

        images["clear sky"] = WayToJPG + "clear_sky.jpg";
        images["few clouds"] = WayToJPG + "few_clouds.jpg";
        images["scattered clouds"] = WayToJPG + "scattered_clouds.jpg";
        images["broken clouds"] = WayToJPG + "broken_сlouds.jpg";
        images["rain"] = WayToJPG + "rain.jpg";
        images["thunderstorm"] = WayToJPG + "thunderstorm.jpg";
        images["snow"] = WayToJPG + "snow.jpg";
        images["fog"] = WayToJPG + "fog.jpg";
        images["light rain"] = WayToJPG + "drizzle.jpg";
        images["haze"] = WayToJPG + "haze.jpg";
        images["dust"] = WayToJPG + "dust.jpg";
        images["sand"] = WayToJPG + "sand.jpg";
        images["squall"] = WayToJPG + "squall.jpg";
        images["tornado"] = WayToJPG + "tornado.jpg";
        defaultImage = WayToJPG + "default.jpg";
    }

    string getImagePath(const string& weatherDescription) const {
        auto it = images.find(weatherDescription);
        if (it != images.end()) {
            return it->second;
        }
        return defaultImage;
    }

private:
    unordered_map<string, string> images;
    string defaultImage;
};

#endif

