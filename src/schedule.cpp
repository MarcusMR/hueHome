#include "schedule.h"

using json = nlohmann::json;

schedule::schedule(control &bridge, std::string schedulePath) : bridge(bridge), schedulePath(schedulePath)
{
    std::ifstream scheduleStringJson(schedulePath);
    if (scheduleStringJson.is_open())
    {
        scheduleStringJson >> scheduleJson;
        scheduleStringJson >> scheduleContents;
    }
    else
    {
        std::cerr << "file did not open" << std::endl;
    }
    scheduleStringJson.close();
}

schedule::~schedule()
{
}

void schedule::updateScheduleIfChange()
{
}

void schedule::scheduleRunning()
{
    while (true)
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm localTime = *std::localtime(&now_time);
        std::ostringstream oss;
        oss << std::put_time(&localTime, "%H:%M");
        std::string time = oss.str();

        if (scheduleJson["schedule"].contains(time))
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
        std::this_thread::sleep_for(std::chrono::minutes(1));
        std::cout << "checking: " << time << std::endl;
    }
}
