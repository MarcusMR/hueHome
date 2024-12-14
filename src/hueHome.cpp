#include <control.h>
#include <schedule.h>
#include <iostream>
#include <curl/curl.h>
#include <json.hpp>
#include <unistd.h>
#include <thread>

using json = nlohmann::json;

//Find the bridges "https://discovery.meethue.com/"
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

    schedule scheduleObj(bridge, "/home/marcus/Documents/projekts/pilipshue/configs/schedule.json");

    std::thread scheduleThread(&schedule::scheduleRunning, &scheduleObj);

    bridge.devices();

    scheduleThread.join();

    return 0;
}