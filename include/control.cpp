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
    CURL* curl = curl_easy_init();
    std::string response;
    std::map<std::string, std::string> rooms;

    if (curl) {
        std::string url = baseUrl + "/resource/bridge_home";  // Endpoint to fetch bridge home data
        std::cout << "DEBUG: Fetching bridge home data from URL: " << url << std::endl;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, 
            +[](void* ptr, size_t size, size_t nmemb, void* userdata) -> size_t {
                ((std::string*)userdata)->append((char*)ptr, size * nmemb);
                return size * nmemb;
            });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "ERROR: CURL Error: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "DEBUG: Response received: " << response << std::endl;

            try {
                // Parse the JSON response
                json json_response = json::parse(response);
                std::cout << "DEBUG: Parsed JSON successfully." << std::endl;

                // Check for errors in the response
                if (json_response.contains("errors") && !json_response["errors"].empty()) {
                    std::cerr << "ERROR: API returned errors: " << json_response["errors"] << std::endl;
                } else {
                    // Extract the data array
                    if (json_response.contains("data")) {
                        const auto& data = json_response["data"];

                        // Iterate through the data array and extract room information
                        for (const auto& item : data) {
                            // Extract the ID of the bridge home and associated children (rooms)
                            std::string home_id = item["id"];
                            std::cout << "DEBUG: Home ID: " << home_id << std::endl;

                            // Iterate over children (which represent the rooms)
                            const auto& children = item["children"];
                            for (const auto& child : children) {
                                std::string room_id = child["rid"];
                                std::string room_type = child["rtype"];
                                std::cout << "DEBUG: Child ID: " << room_id << ", Type: " << room_type << std::endl;

                                // Assuming that rooms are of type 'room', store them in the map
                                if (room_type == "room") {
                                    rooms[room_id] = "Room ID: " + room_id;  // Map room ID to a string description
                                }
                            }
                        }
                    } else {
                        std::cerr << "ERROR: No 'data' field found in the API response." << std::endl;
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "ERROR: Failed to parse JSON: " << e.what() << std::endl;
            }
        }

        curl_easy_cleanup(curl);
    } else {
        std::cerr << "ERROR: Failed to initialize CURL for fetching bridge home data." << std::endl;
    }

    return rooms;
}

