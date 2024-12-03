#include <control.h>
#include <iostream>
#include <curl/curl.h>
#include <json.hpp>
#include <unistd.h>
#include <fstream>
#include <ctime>
#include <sstream>
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

void schedule(control &bridge)
{
    json scheduleJson;
    std::ifstream scheduleStringJson("/home/marcus/rapos/hueHome/configs/schedule.json");
    if (scheduleStringJson.is_open())
    {
        scheduleStringJson >> scheduleJson;
    }
    else
    {
        std::cerr << "file did not open" << std::endl;
    }
    scheduleStringJson.close();

    while (true)
    {
        std::time_t now = std::time(nullptr);
        std::tm *localtime = std::localtime(&now);

        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << localtime->tm_hour
            << ":" << std::setw(2) << std::setfill('0') << localtime->tm_min;

        std::string time = oss.str();

        time = "21:21";

        std::cout << scheduleJson["schedule"][time] << std::endl;

        bool s = scheduleJson["schedule"].contains(time);

        std::cout << std::boolalpha << s << std::endl;

        if (s)
        {
            json event = scheduleJson["schedule"][time];
            std::string lightID = event["light_id"];
            bool on = event["action"]["on"];
            int brightness = event["action"].value("brightness", 100);

            std::cout << lightID << std::endl;

            if (on)
            {
                bridge.turnOnGroup(lightID, brightness);
            }
            else
            {
                bridge.turnOffGroup(lightID);
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "checking: " << time << std::endl;
    }
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

    schedule(bridge);

    return 0;
}