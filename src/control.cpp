#include "control.h"
#include <iostream>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

control::control(std::string hueBridgeIp, std::string accessToken) : hueBridgeIp(hueBridgeIp), accessToken(accessToken)
{
    baseUrl = "https://" + hueBridgeIp + "/clip/v2";

    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL." << std::endl;
    } else {
        std::cout << "CURL initialized successfully." << std::endl;
    }
    this->getRooms();
}

control::~control() {
    if (curl) {
        curl_easy_cleanup(curl);
        std::cout << "CURL cleaned up successfully." << std::endl;
    }
}

std::string control::getDevice(const std::string& id) {
    auto it = roomsMap.find(id);
    if (it != roomsMap.end()) {
        std::cout << "DEBUG: Found id " << id << " -> " << it->second << std::endl;
        return it->second;
    }
    std::cerr << "ERROR: ID not found in roomsMap: " << id << std::endl;
    return "";
}

void control::turnOnGroup(const std::string& id, int brightness) {
    std::string payload = "{\"on\": {\"on\": true}, \"dimming\": {\"brightness\": " + std::to_string(brightness) + "}}";
    CURLcode res;
    std::string url = baseUrl + "/resource/grouped_light/" + this->getDevice(id);

    curl_easy_reset(curl);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("hue-application-key: " + accessToken).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "ERROR: Error turning on light: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "DEBUG: Light turned on successfully with brightness " << brightness << "%" << std::endl;
    }

    curl_slist_free_all(headers);
}


void control::turnOffGroup(const std::string& id) {
    std::string payload = "{\"on\": {\"on\": false}}";
    CURLcode res;
    std::string url = baseUrl + "/resource/grouped_light/" + this->getDevice(id);

    std::cout << "DEBUG: Turning off light group with ID: " << id << std::endl;
    std::cout << "DEBUG: Constructed URL: " << url << std::endl;
    std::cout << "DEBUG: Payload: " << payload << std::endl;

    // Set up a string to capture the response
    std::string responseData;

    curl_easy_reset(curl);

    // Set up CURL options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    
    // Set the headers (authentication key)
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("hue-application-key: " + accessToken).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    // Set up SSL verification options
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    // Set the write callback function to capture the response
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

    // Perform the request
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "ERROR: Error turning off light group: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "DEBUG: Light group turned off successfully" << std::endl;
        std::cout << "DEBUG: Response Data: " << responseData << std::endl;
    }
}

void control::getRooms() {
    std::string url = "https://" + hueBridgeIp + "/clip/v2/resource/room";
    std::string response;

    curl_easy_reset(curl);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, ("hue-application-key: " + accessToken).c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Request failed: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(headers);
        return; // Stop videre eksekvering
    }

    curl_slist_free_all(headers);

    try {
        json rooms = json::parse(response);
        for (const auto& room : rooms["data"]) {
            roomsMap[room["metadata"]["name"]] = room["services"][0]["rid"];
        }

        for (const auto& room : roomsMap) {
            std::cout << "Room: " << room.first << ", Service ID: " << room.second << std::endl;
        }
    } catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
    }
}
