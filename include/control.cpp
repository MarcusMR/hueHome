#include "control.h"
#include <iostream>

control::control() {
    const char* token = std::getenv("ACCESS_TOKEN");

    if (token){
        std:: string accessToken(token);

        std::cout << "succesfuly retrived accesstoken: " << accessToken << std::endl;

        ACCESS_TOKEN = accessToken;
    }
    else{
        std::cerr << "Missing access token" << std::endl;
    }

    baseUrl = "http://" + HUE_BRIDGE_IP + "/api/" + ACCESS_TOKEN;  // Initialize baseUrl
    curl = curl_easy_init();  // Initialize CURL
    if (!curl) {
        std::cerr << "Failed to initialize CURL." << std::endl;
    } else {
        std::cout << "CURL initialized successfully." << std::endl;
    }
}

control::~control() {
    if (curl) {
        curl_easy_cleanup(curl);  // Clean up CURL
        std::cout << "CURL cleaned up successfully." << std::endl;
    }
}

// Callback function to capture response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userData) {
    size_t totalSize = size * nmemb;
    userData->append((char*)contents, totalSize);
    return totalSize;
}

CURLcode control::getBridge() {
    CURLcode res;
    std::string url = "https://discovery.meethue.com/";
    std::string response;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Perform the request
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "ERROR: Failed to perform request: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "DEBUG: Request performed successfully." << std::endl;
    }

    std::cout << response << std::endl;
    return res;
}


void control::turnOnLights(const std::string& id) {
    std::string payload = "{\"on\": true}";
    CURLcode res;
    std::string url = baseUrl + "/lights/" + id + "/state";

    // Debugging output
    std::cout << "DEBUG: Turning on light with ID: " << id << std::endl;
    std::cout << "DEBUG: Constructed URL: " << url << std::endl;
    std::cout << "DEBUG: Payload: " << payload << std::endl;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");  // Set request method to PUT
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);  // Optional: Set headers if needed

    res = curl_easy_perform(curl);  // Perform the request
    if (res != CURLE_OK) {
        std::cerr << "ERROR: Error turning on light: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "DEBUG: Light turned on successfully." << std::endl;
    }
}

void control::turnOffLights(const std::string& id) {
    std::string payload = "{\"on\": false}";
    CURLcode res;
    std::string url = baseUrl + "/lights/" + id + "/state";

    // Debugging output
    std::cout << "DEBUG: Turning off light with ID: " << id << std::endl;
    std::cout << "DEBUG: Constructed URL: " << url << std::endl;
    std::cout << "DEBUG: Payload: " << payload << std::endl;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");  // Use PUT method for changing state
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, NULL);

    res = curl_easy_perform(curl);  // Perform the request
    if (res != CURLE_OK) {
        std::cerr << "ERROR: Error turning off light: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "DEBUG: Light turned off successfully." << std::endl;
    }
}

// Method to fetch and extract rooms data
std::map<std::string, std::string> control::fetchRooms() {
    std::map<std::string, std::string> rooms;

    return rooms;
}

