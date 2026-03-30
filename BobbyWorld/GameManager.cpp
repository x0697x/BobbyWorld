#include "GameManager.hpp"
#include <cmath>

GameManager::GameManager() : eatenCount(0), totalTime(0.f) {}

void GameManager::update(float dt, Player& player) {
	totalTime += dt;
	manageProgression(player);

    float mass = (player.getRadius() * player.getRadius()) / 10.f;

    // Sparkles spawn
    if (mass >= 30000.f) {
        spawnTimer += dt;
        if (spawnTimer >= 0.1f) {
            float playerRad = player.getRadius();

            // Create the particle
            Particle p;

            // Calculate dynamic size based on Bobby
            float sparkleSize = playerRad * 0.05f;
            if (sparkleSize < 2.f) sparkleSize = 1.5f; // Minimum size
            if (sparkleSize > 10.f) sparkleSize = 1.5f; // Optional: Maximum size cap

            p.shape.setRadius(sparkleSize);
            p.shape.setOrigin({ sparkleSize, sparkleSize });
            p.shape.setFillColor(sf::Color::Yellow);

            // Position logic
            float angle = (rand() % 360) * 3.14159f / 180.f;
            float dist = playerRad * 0.8f;

            sf::Vector2f playerPos = player.getPosition();
            sf::Vector2f offset(std::cos(angle) * dist, std::sin(angle) * dist);
            p.shape.setPosition(playerPos + offset);

            // Physics & life
            p.velocity = sf::Vector2f(static_cast<float>(rand() % 40 - 20),
                static_cast<float>(rand() % 40 - 20));
            p.lifetime = 1.0f;

            sparkles.push_back(p);
            spawnTimer = 0.f;
        }
    }

    // Update sparkles
    for (size_t i = 0; i < sparkles.size(); ) {
        sparkles[i].lifetime -= dt;
        if (sparkles[i].lifetime <= 0) {
            sparkles.erase(sparkles.begin() + i);
        }
        else {
            sparkles[i].shape.move(sparkles[i].velocity * dt);
            // Fade out
            sf::Color c = sparkles[i].shape.getFillColor();
            c.a = static_cast<std::uint8_t>(255 * (sparkles[i].lifetime / 1.0f));
            sparkles[i].shape.setFillColor(c);
            i++;
        }
    }
}

void GameManager::manageProgression(Player& player) {
    float radius = player.getRadius();
    float mass = (radius * radius) / 10.f;

    // Tier 2
    if (mass >= 15000.f) {
        player.setColor(sf::Color(75, 0, 130));   // Indigo
    }
    else if (mass >= 12500.f) {
        player.setColor(sf::Color(0, 100, 0));    // Deep Emerald
    }
    else if (mass >= 10000.f) {
        player.setColor(sf::Color(0, 0, 139));    // Dark Sapphire
    }
    else if (mass >= 7500.f) {
        player.setColor(sf::Color(139, 0, 0));    // Dark Ruby
    }
    // Tier 1
    else if (mass >= 5000.f) {
        player.setColor(sf::Color(229, 228, 226)); // Platinum
    }
    else if (mass >= 2500.f) {
        player.setColor(sf::Color(218, 165, 32));  // Soft Gold
    }
    else if (mass >= 1000.f) {
        player.setColor(sf::Color(192, 192, 192)); // Silver
    }
    else if (mass >= 500.f) {
        player.setColor(sf::Color(169, 169, 169)); // Dark Grey
    }
    else {
        player.setColor(sf::Color::White);         // Default
    }
}

void GameManager::drawSparkles(sf::RenderWindow& window) {
    for (auto& p : sparkles) {
        window.draw(p.shape);
    }
}