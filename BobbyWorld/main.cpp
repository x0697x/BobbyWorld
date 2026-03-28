#include <SFML/Graphics.hpp>
#include <vector> // Stars
#include <random>
#include "Player.hpp"
#include "Bot.hpp"
#include <cmath>
#include "HUD.hpp"

int main() {
	// Main window
	sf::RenderWindow window(sf::VideoMode({ 1280, 720 }), "BobbyWorld");
	window.setFramerateLimit(60);

	// Background
	std::vector<sf::CircleShape> stars;
	std::mt19937 gen(1337); // Random number generator
	std::uniform_real_distribution<float> distX(-500.f, 2000.f);
	std::uniform_real_distribution<float> distY(-500.f, 2000.f);

	for (int i = 0; i < 500; ++i) {
		sf::CircleShape star(1.5f); // Star size
		star.setFillColor(sf::Color(150, 150, 150)); // Star color
		star.setPosition({ distX(gen), distY(gen) });
		stars.push_back(star);
	}

	// Player
	Player bobby(20.0f, sf::Color::White);
	sf::View view(sf::FloatRect({ 0, 0 }, { 1280, 800 }));
	sf::Clock clock;

	// Bots
	std::vector<Bot> bots;
	// Initial spawn
	for (int i = 0; i < 40; ++i) {
		std::uniform_real_distribution<float> posDist(-1000.f, 2000.f);
		std::uniform_real_distribution<float> sizeDist(10.f, 40.f);
		bots.emplace_back(sizeDist(gen), sf::Vector2f{ posDist(gen), posDist(gen) }, gen);
	}

    //HUD
    HUD hud;

	// Main game loop
	while (window.isOpen()) {
		float dt = clock.restart().asSeconds();

		while (const std::optional event = window.pollEvent()) {
			// Check if event closed
			if (event->is<sf::Event::Closed>())
				window.close();
		}

		// Update player
		bobby.update(dt);

        // ZoomFactor
        float zoomFactor = 1.0f + (bobby.getRadius() - 20.0f) / 200.0f;

		// Main proc loop (move, color, collide)
        for (int i = 0; i < bots.size(); ++i) {
            // Recycling spawn
            sf::Vector2f dToB = bobby.getPosition() - bots[i].getPosition();
            float distSq = dToB.x * dToB.x + dToB.y * dToB.y;
            float maxVisibleDist = 3500.f * zoomFactor;

            // If bot > 5000 units away, remove it
            if (distSq > maxVisibleDist * maxVisibleDist) {
                bots.erase(bots.begin() + i);
                --i;
                continue;
            }

            // Bot update
            bots[i].update(dt, gen);

            // HUD update
            hud.update(dt, bobby.getRadius(), (int)bots.size());

            // Movement and Color
            if (bots[i].getRadius() > bobby.getRadius()) bots[i].setColor(sf::Color::Red);
            else bots[i].setColor(sf::Color::White);

            // Bobby vs Bot Collision
            float distance = std::sqrt(distSq);
            if (distance < bobby.getRadius() + bots[i].getRadius()) {
                if (bobby.getRadius() > bots[i].getRadius()) {
                    bobby.grow(bots[i].getRadius() * 0.15f); // Growth factor
                    bots.erase(bots.begin() + i);
                    --i;
                    continue;
                }
                else {
                    window.close(); // Game Over
                }
            }

            // Bot vs Bot Collision
            for (int j = i + 1; j < bots.size(); ++j) {
                sf::Vector2f bDiff = bots[i].getPosition() - bots[j].getPosition();
                float bDist = std::sqrt(bDiff.x * bDiff.x + bDiff.y * bDiff.y);

                if (bDist < bots[i].getRadius() + bots[j].getRadius()) {
                    if (bots[i].getRadius() > bots[j].getRadius()) {
                        // Cap bot growth
                        float growth = bots[j].getRadius() * 0.1f;
                        if (bots[i].getRadius() < bobby.getRadius() * 2.0) {
                            bots[i].setRadius(bots[i].getRadius() + growth);
                        }
                        bots.erase(bots.begin() + j);
                        --j;
                    } else {
                        float growth = bots[i].getRadius() * 0.1f;
                        if (bots[j].getRadius() < bobby.getRadius() * 2.0f) {
                            bots[j].setRadius(bots[j].getRadius() + growth);
                        }
                        bots.erase(bots.begin() + i);
                        --i;
                        break;
                    }
                }
            }
        }

        // 3. SPAWNING LOOP (With Safe-Zone)
        if (bots.size() < 40) {
            // Spawn outside Bobby zone but inside recycling zone
            float minSpawnDist = 1500.f * zoomFactor;
            float maxSpawnDist = 3000.f * zoomFactor;

            std::uniform_real_distribution<float> angleDist(0, 2.f * 3.14159f);
            std::uniform_real_distribution<float> distDist(minSpawnDist, maxSpawnDist);

            float angle = angleDist(gen);
            float radius = distDist(gen);
            sf::Vector2f spawnPos = bobby.getPosition() + sf::Vector2f(cos(angle) * radius, sin(angle) * radius);

            // Size scaling
            float minS = std::max(10.f, bobby.getRadius() * 0.4f);
            float maxS = std::max(40.f, bobby.getRadius() * 2.5f);

            // Favors larger numbers
            std::uniform_real_distribution<float> sizeDist(minS, maxS);
            float finalSize = sizeDist(gen);

            // 10% chance to spawn Alpha bot
            if (std::uniform_real_distribution<float>(0, 1)(gen) > 0.9f) {
                finalSize *= 1.5f;
            }

            bots.emplace_back(sizeDist(gen), spawnPos, gen);
        }

        // 4. CAMERA & RENDERING
        view.setSize({ 1280 * zoomFactor, 720 * zoomFactor });
        view.setCenter(bobby.getPosition());

        window.clear(sf::Color::Black);
        window.setView(view);

        float worldSize = 2500.0f;
        for (const auto& star : stars) {
            sf::Vector2f pos = star.getPosition();
            sf::Vector2f vPos = view.getCenter();
            float x = std::fmod(pos.x - vPos.x + worldSize / 2.0f, worldSize);
            if (x < 0) x += worldSize;
            float y = std::fmod(pos.y - vPos.y + worldSize / 2.0f, worldSize);
            if (y < 0) y += worldSize;
            sf::CircleShape ts = star;
            ts.setPosition({ x + vPos.x - worldSize / 2.0f, y + vPos.y - worldSize / 2.0f });
            window.draw(ts);
        }

        for (auto& bot : bots) bot.draw(window);
        bobby.draw(window);
        hud.draw(window);
        window.display();
    }
    return 0;
}