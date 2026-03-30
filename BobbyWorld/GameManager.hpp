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
	void update(float dt, Player& player, sf::Texture& tex1, sf::Texture& tex2, sf::Texture& tex3, sf::Texture& tex4, sf::Texture& tex5, sf::Texture& tex6, sf::Texture& tex7, sf::Texture& tex8);

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

	// Helper for color progression
	void manageProgression(Player& player, sf::Texture& tex1, sf::Texture& tex2, sf::Texture& tex3, sf::Texture& tex4, sf::Texture& tex5, sf::Texture& tex6, sf::Texture& tex7, sf::Texture& tex8);
};

#endif