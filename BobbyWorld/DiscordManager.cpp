#include "DiscordManager.hpp"
#include <string>
#include <ctime>

DiscordManager::DiscordManager() {
    // Replace with your actual Client ID from the portal
    auto result = discord::Core::Create(1487603160880251050, DiscordCreateFlags_Default, &core);
    if (result != discord::Result::Ok) {
        std::cout << "Discord RPC Failed to initialize." << std::endl;
    }
}

DiscordManager::~DiscordManager() {
    delete core;
}

void DiscordManager::update(float mass, int eaten) {
    if (!core) return;

    discord::Activity activity{};
    activity.SetDetails(("Mass: " + std::to_string((int)mass)).c_str());
    activity.SetState(("Eaten: " + std::to_string(eaten)).c_str());

    // Timer since the game started
    activity.GetTimestamps().SetStart(time(0));

    // Images (matches the name you uploaded to the portal)
    activity.GetAssets().SetLargeImage("bobby_large");
    activity.GetAssets().SetLargeText("Bobby is growing!");

    core->ActivityManager().UpdateActivity(activity, [](discord::Result result) {
        // Optional: handle result
        });
}

void DiscordManager::runCallbacks() {
    if (core) core->RunCallbacks();
}