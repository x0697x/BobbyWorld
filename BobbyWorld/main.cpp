#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include "Player.hpp"
#include "Bot.hpp"
#include <cmath>
#include "HUD.hpp"
#include "GameManager.hpp"
#include "DiscordManager.hpp"

int main() {
    // 1. Settings with Antialiasing
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    // 2. Window (Fixed constructor: added sf::Style::Default)
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1280, 720)), "BobbyWorld", sf::State::Windowed, settings);
    window.setFramerateLimit(60);

    // Background
    std::vector<sf::CircleShape> stars;
    std::mt19937 gen(1337);
    std::uniform_real_distribution<float> distX(0.0f, 1.0f);
    std::uniform_real_distribution<float> distY(0.0f, 1.0f);

    for (int i = 0; i < 1500; ++i) {
        sf::CircleShape star(1.5f);
        star.setFillColor(sf::Color(150, 150, 150));
        star.setPosition({ distX(gen), distY(gen) });
        stars.push_back(star);
    }

    Player bobby(20.0f, sf::Color::White);
    sf::View view(sf::FloatRect({ 0, 0 }, { 1280, 800 }));
    sf::Clock clock;

    // --- INITIAL BOTS SPAWNING ---
    std::vector<Bot> bots;
    for (int i = 0; i < 40; ++i) {
        std::uniform_real_distribution<float> posDist(-2000.f, 2000.f); // Wider range
        std::uniform_real_distribution<float> sizeDist(10.f, 40.f);

        sf::Vector2f spawnPos;
        float minSafeDistance = 600.f; // Bot dist from Bobby at start

        // Keep gen pos until outside safezone
        do {
            spawnPos = { posDist(gen), posDist(gen) };
            float distToCenter = std::sqrt(spawnPos.x * spawnPos.x + spawnPos.y * spawnPos.y);
            if (distToCenter > minSafeDistance) break;
        } while (true);

        bots.emplace_back(sizeDist(gen), spawnPos, gen);
    }

    HUD hud;
    GameManager gm;
    DiscordManager discordRPC;
    float rpcTimer = 0.f;

    // --- MAIN GAME LOOP ---
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        if (window.hasFocus()) {
            bobby.update(dt);
        }

        float zoomFactor = 1.0f + (bobby.getRadius() - 20.0f) / 200.0f;

        // --- BOTS LOGIC ---
        for (int i = 0; i < bots.size(); ++i) {
            sf::Vector2f dToB = bobby.getPosition() - bots[i].getPosition();
            float distSq = dToB.x * dToB.x + dToB.y * dToB.y;
            float maxVisibleDist = 3500.f * zoomFactor;

            if (distSq > maxVisibleDist * maxVisibleDist) {
                bots.erase(bots.begin() + i);
                --i;
                continue;
            }

            bots[i].update(dt, bobby.getPosition(), gen);

            float distance = std::sqrt(distSq);
            if (distance < bobby.getRadius() + bots[i].getRadius()) {
                if (bobby.getRadius() > bots[i].getRadius()) {
                    bobby.grow(bots[i].getRadius() * 0.15f);
                    bots.erase(bots.begin() + i);
                    gm.addEaten();
                    --i;
                    continue;
                }
                else {
                    window.close(); // Game Over
                }
            }

            for (int j = i + 1; j < bots.size(); ++j) {
                sf::Vector2f bDiff = bots[i].getPosition() - bots[j].getPosition();
                float bDistSq = bDiff.x * bDiff.x + bDiff.y * bDiff.y;
                float combinedR = bots[i].getRadius() + bots[j].getRadius();

                if (bDistSq < combinedR * combinedR) {
                    if (bots[i].getRadius() > bots[j].getRadius()) {
                        if (bots[i].getRadius() < bobby.getRadius() * 2.0f) {
                            bots[i].setRadius(bots[i].getRadius() + bots[j].getRadius() * 0.1f);
                        }
                        bots.erase(bots.begin() + j);
                        --j;
                    }
                    else {
                        if (bots[j].getRadius() < bobby.getRadius() * 2.0f) {
                            bots[j].setRadius(bots[j].getRadius() + bots[i].getRadius() * 0.1f);
                        }
                        bots.erase(bots.begin() + i);
                        --i;
                        break;
                    }
                }
            }
        }

        // --- SPAWNING ---
        if (bots.size() < 40) {
            float minSpawnDist = 1500.f * zoomFactor;
            float maxSpawnDist = 3000.f * zoomFactor;
            std::uniform_real_distribution<float> angleDist(0, 2.f * 3.14159f);
            std::uniform_real_distribution<float> distDist(minSpawnDist, maxSpawnDist);

            float angle = angleDist(gen);
            float radius = distDist(gen);
            sf::Vector2f spawnPos = bobby.getPosition() + sf::Vector2f(cos(angle) * radius, sin(angle) * radius);

            float minS = std::max(10.f, bobby.getRadius() * 0.4f);
            float maxS = std::max(40.f, bobby.getRadius() * 2.5f);
            std::uniform_real_distribution<float> sizeDist(minS, maxS);
            float finalSize = sizeDist(gen);

            // Check if spawn is Alpha (10% chance)
            if (std::uniform_real_distribution<float>(0, 1)(gen) > 0.9f) {
                finalSize *= 1.5f;
                bots.emplace_back(finalSize, spawnPos, gen); // Create bot
                bots.back().setAlpha(true); // Flag aggressive
                //bots.back().setColor(sf::Color::Red); // Alpha color
            }
            else {
                bots.emplace_back(finalSize, spawnPos, gen);
            }
        }

        // --- DISCORD & HUD UPDATES ---
        gm.update(dt, bobby);
        hud.update(dt, bobby.getRadius(), static_cast<int>(bots.size()), gm.getEatenCount(), gm.getTotalTime());
        discordRPC.runCallbacks();

        rpcTimer += dt;
        if (rpcTimer >= 5.0f) {
            float currentMass = (bobby.getRadius() * bobby.getRadius()) / 10.f;
            discordRPC.update(currentMass, gm.getEatenCount());
            rpcTimer = 0.f;
        }

        // --- RENDERING ---
        view.setSize({ 1280 * zoomFactor, 720 * zoomFactor });
        view.setCenter(bobby.getPosition());

        window.clear(sf::Color::Black);
        window.setView(view);

        // Infinite Background Logic
        float baseWorldSize = 4000.0f;
        float dynamicWorldSize = baseWorldSize * zoomFactor;

        for (const auto& star : stars) {
            // Treat the star's stored position as a percentage (0.0 to 1.0) of the world
            sf::Vector2f relativePos = star.getPosition();
            sf::Vector2f absoluteStarPos = { relativePos.x * dynamicWorldSize, relativePos.y * dynamicWorldSize };

            sf::Vector2f vPos = view.getCenter();

            // The tiling math
            float x = std::fmod(absoluteStarPos.x - vPos.x + dynamicWorldSize / 2.0f, dynamicWorldSize);
            if (x < 0) x += dynamicWorldSize;

            float y = std::fmod(absoluteStarPos.y - vPos.y + dynamicWorldSize / 2.0f, dynamicWorldSize);
            if (y < 0) y += dynamicWorldSize;

            sf::CircleShape ts = star;
            ts.setRadius(1.5f * (zoomFactor * 0.5f + 0.5f));
            ts.setPosition({ x + vPos.x - dynamicWorldSize / 2.0f, y + vPos.y - dynamicWorldSize / 2.0f });
            window.draw(ts);
        }

        for (auto& bot : bots) bot.draw(window);
        bobby.draw(window);
        gm.drawSparkles(window);

        window.setView(window.getDefaultView());
        hud.draw(window);
        window.display();
    }

    return 0;
}