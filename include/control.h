#ifndef CONTROL_H
#define CONTROL_H

#include <string>
#include <curl/curl.h>
#include <map>
#include <nlohmann/json.hpp>
#include <cstdlib>

using json = nlohmann::json;

class control
{
private:
    const std::string HUE_BRIDGE_IP = "192.168.28.26";                            // Replace with your Hue Bridge IP
    std::string ACCESS_TOKEN; // Replace with your access token
    std::string baseUrl; // Initialize this in the constructor
    CURL *curl;

public:
    control();  // Constructor declaration
    ~control(); // Destructor declaration

    CURLcode getBridge();
    void turnOnLights(const std::string& id);
    void turnOffLights(const std::string& id);

    std::map<std::string, std::string> fetchRooms(); // New method to fetch rooms
};

#endif // CONTROL_H
