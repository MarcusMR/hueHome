#include <control.h>
#include <schedule.h>
#include <iostream>
#include <curl/curl.h>
#include <json.hpp>
#include <unistd.h>
#include <thread>

using json = nlohmann::json;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userData)
{
    size_t totalSize = size * nmemb;
    userData->append((char *)contents, totalSize);
    return totalSize;
}

void getBridge(json &bridges)
{
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

    if (res != CURLE_OK)
    {
        std::cerr << "ERROR: Failed to perform request: " << curl_easy_strerror(res) << std::endl;
    }
    else
    {
        std::cout << "DEBUG: Request performed successfully." << std::endl;
    }

    curl_easy_cleanup(curl);

    std::cout << response << std::endl;

    bridges = json::parse(response);
}

int main()
{
    const char *token = std::getenv("ACCESS_TOKEN");
    if (!token)
    {
        std::cerr << "Missing access token" << std::endl;
        return 1;
    }

    std::cout << "Successfully retrieved access token: " << token << std::endl;

    control bridge("192.168.28.26", token);

    schedule s(bridge, "/home/marcus/rapos/hueHome/configs/schedule.json");

    s.scheduleRunning();

    // bridge.turnOnGroup("Gang", 100);

    return 0;
}