#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include <SFML/Graphics.hpp>
#include "Player.hpp"

// Particles
struct Particle {
	sf::CircleShape shape;
	sf::Vector2f velocity;
	float lifetime; // Lasts for
};

class GameManager {
public:
	GameManager();

	// Update game stats
	void update(float dt, Player& player);

	// Getters HUD
	int getEatenCount() const { return eatenCount; }
	float getTotalTime() const { return totalTime; }
	void addEaten() { eatenCount++; }

	// Draw sparkles
	void drawSparkles(sf::RenderWindow& window);

private:
	std::vector<Particle> sparkles;
	float spawnTimer = 0.f;
	int eatenCount;
	float totalTime;

	// Helper with shiny colors
	void manageProgression(Player& player);
};

#endif