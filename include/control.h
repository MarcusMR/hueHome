#ifndef CONTROL_H
#define CONTROL_H

#include <string>
#include <curl/curl.h>
#include <map>
#include <json.hpp>
#include <cstdlib>

using json = nlohmann::json;

class control
{
private:
    std::string hueBridgeIp;
    std::string accessToken;
    std::string baseUrl;
    std::map<std::string, std::string> roomsMap;
    CURL *curl;
public:
    control(std::string hueBridgeIp, std::string accessToken);
    ~control();

    std::string getDevice(const std::string& id);
    void turnOnGroup(const std::string& id, int brightness);
    void turnOffGroup(const std::string& id);
    void devices();
    void getRooms();
};

#endif // CONTROL_H
