#ifndef DISCORDMANAGER_HPP
#define DISCORDMANAGER_HPP

#include "discord.h"
#include <iostream>

class DiscordManager {
public:
    DiscordManager();
    ~DiscordManager();

    void update(float mass, int eaten);
    void runCallbacks();

private:
    discord::Core* core{ nullptr };
};

#endif