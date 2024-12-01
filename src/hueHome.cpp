#include <control.h>
#include <iostream>
#include <curl/curl.h>
#include <json.hpp>
#include <unistd.h>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userData) {
    size_t totalSize = size * nmemb;
    userData->append((char*)contents, totalSize);
    return totalSize;
}

void getBridge(json &bridges) {
    CURL *curl;
    curl = curl_easy_init();

    CURLcode res;
    std::string url = "https://discovery.meethue.com/";
    std::string response;
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        std::cerr << "ERROR: Failed to perform request: " << curl_easy_strerror(res) << std::endl;
    } else {
        std::cout << "DEBUG: Request performed successfully." << std::endl;
    }

    curl_easy_cleanup(curl);
    
    std::cout << response << std::endl;

    bridges = json::parse(response);
}

int main() {
    const char* token = std::getenv("ACCESS_TOKEN");

    if (!token) {
        std::cerr << "Missing access token" << std::endl;
        return 1;
    }

    std::cout << "Successfully retrieved access token: " << token << std::endl;
    
    json bridges;
    //getBridge(bridges);

    //if (bridges.is_null()){
    //    std::cerr << "Missing json obj" << std::endl;
    //}
    //bridges[1]["internalipaddress"]
    control bridge("192.168.28.26", token);

    bridge.getRooms();
    bridge.turnOffGroup("8c62f387-b7ce-4c71-9cf1-cf12e5309775");
    sleep(1);
    bridge.turnOnGroup("8c62f387-b7ce-4c71-9cf1-cf12e5309775", 100);

    return 0;
}