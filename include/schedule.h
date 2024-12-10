#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <json.hpp>
#include <chrono>
#include <control.h>
#include <string>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>

using json = nlohmann::json;
using namespace std::chrono;

class schedule
{
private:
    std::string schedulePath;
    std::string scheduleContents;
    json scheduleJson;
    control bridge;
public:
    schedule(control &bridge, std::string schedulePath);
    ~schedule();

    void updateScheduleIfChange();
    void scheduleRunning();
};
#endif