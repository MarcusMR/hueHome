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
}

control::~control() {
    if (curl) {
        curl_easy_cleanup(curl);
        std::cout << "CURL cleaned up successfully." << std::endl;
    }
}

void control::turnOnLights(const std::string& id, int brightness) {
    std::string payload = "{\"on\": true, \"dimming\": {\"brightness\": " + std::to_string(brightness) + "}}";
    CURLcode res;
    std::string url = baseUrl + "/resource/light/" + id; // Ensure the correct URL format

    std::cout << "DEBUG: Turning on light with ID: " << id << std::endl;
    std::cout << "DEBUG: Constructed URL: " << url << std::endl;
    std::cout << "DEBUG: Payload: " << payload << std::endl;

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
}


void control::turnOffLights(const std::string& id) {
    std::string payload = "{\"on\": false}";
    CURLcode res;
    std::string url = baseUrl + "/resource/light/" + id;

    std::cout << "DEBUG: Turning on light with ID: " << id << std::endl;
    std::cout << "DEBUG: Constructed URL: " << url << std::endl;
    std::cout << "DEBUG: Payload: " << payload << std::endl;

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
        std::cerr << "ERROR: Error turning off light: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "DEBUG: Light turned off successfully" << std::endl;
    }
}

void control::turnOnGroup(const std::string& id, int brightness) {
    std::string payload = "{\"on\": {\"on\": true}, \"dimming\": {\"brightness\": " + std::to_string(brightness) + "}}";
    CURLcode res;
    std::string url = baseUrl + "/resource/grouped_light/" + id; // Ensure the correct URL format

    std::cout << "DEBUG: Turning on light with ID: " << id << std::endl;
    std::cout << "DEBUG: Constructed URL: " << url << std::endl;
    std::cout << "DEBUG: Payload: " << payload << std::endl;

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
}

void control::turnOffGroup(const std::string& id) {
    std::string payload = "{\"on\": {\"on\": false}}";
    CURLcode res;
    std::string url = baseUrl + "/resource/grouped_light/" + id;

    std::cout << "DEBUG: Turning off light group with ID: " << id << std::endl;
    std::cout << "DEBUG: Constructed URL: " << url << std::endl;
    std::cout << "DEBUG: Payload: " << payload << std::endl;

    // Set up a string to capture the response
    std::string responseData;

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
        std::cout << "DEBUG: Response Data: " << responseData << std::endl; // Print the response
    }
}

void control::getRooms() {
    std::string url = "https://" + hueBridgeIp + "/clip/v2/resource/room";
    std::string response;

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
    } else {
        std::cout << "Response: " << response << std::endl;
    }

    json rooms;
    rooms = json::parse(response);
    for (const auto& room : rooms["data"]){
        roomsMap[room["metadata"]["name"]] = room["services"][0]["rid"];
    }

    for (const auto& room : roomsMap) {
        std::cout << "Room: " << room.first << ", Service ID: " << room.second << std::endl;
    }
}