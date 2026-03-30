#ifndef BOT_HPP
#define BOT_HPP

#include <SFML/Graphics.hpp>
#include <random>

class Bot {
public:
	// Gen by ref
	Bot(float radius, sf::Vector2f position, std::mt19937& gen);

	void update(float dt, sf::Vector2f playerPos, std::mt19937& gen);
	void draw(sf::RenderWindow& window);

	// Growth & coll helper
	sf::Vector2f getPosition() const { return shape.getPosition(); }
	float getRadius() const { return shape.getRadius(); }
	void setRadius(float newRadius);
	void setColor(sf::Color color);

	// Alpha
	bool isAlpha = false;
	void setAlpha(bool status) { isAlpha = status; }

private:
	sf::CircleShape shape;
	sf::Vector2f direction;
	float speed;
	float changeDirTimer; // Bot change direction every * seconds
};

#endif