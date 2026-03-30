#include "GameManager.hpp"
#include <cmath>

GameManager::GameManager() : eatenCount(0), totalTime(0.f) {}

void GameManager::update(float dt, Player& player, sf::Texture& tex1, sf::Texture& tex2, sf::Texture& tex3, sf::Texture& tex4, sf::Texture& tex5, sf::Texture& tex6, sf::Texture& tex7, sf::Texture& tex8) {
    totalTime += dt;
    manageProgression(player, tex1, tex2, tex3, tex4, tex5, tex6, tex7, tex8);

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

void GameManager::manageProgression(Player& player, sf::Texture& tex1, sf::Texture& tex2, sf::Texture& tex3, sf::Texture& tex4, sf::Texture& tex5, sf::Texture& tex6, sf::Texture& tex7, sf::Texture& tex8) {
    float mass = (player.getRadius() * player.getRadius()) / 10.f;

    if (mass >= 100000.f) {
        player.setTexture(tex8);
    }
    else if (mass >= 50000.f) {
        player.setTexture(tex7);
    }
    else if (mass >= 25000.f) {
        player.setTexture(tex6);
    }
    else if (mass >= 15500.f) {
        player.setTexture(tex5);
    }
    // Tier 1
    else if (mass >= 10000.f) {
        player.setTexture(tex4);
    }
    else if (mass >= 7500.f) {
        player.setTexture(tex3);
    }
    else if (mass >= 5000.f) {
        player.setTexture(tex2);
    }
    else if (mass >= 2500.f) {
        player.setTexture(tex1);
    }
    else {
        player.setColor(sf::Color::White);
    }
}

void GameManager::drawSparkles(sf::RenderWindow& window) {
    for (auto& p : sparkles) {
        window.draw(p.shape);
    }
}