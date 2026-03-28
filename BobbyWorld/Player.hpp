#ifndef PLAYER_HPP
#define Player_HPP

#include <SFML/Graphics.hpp>

class Player {
public:
	Player(float radius, sf::Color color);
	void update(float dt); // Movement logic
	void draw(sf::RenderWindow& window); // Rendering
	void grow(float amount);

	// Camera and collision
	sf::Vector2f getPosition() const { return shape.getPosition(); }
	float getRadius() const { return shape.getRadius(); }

private:
	sf::CircleShape shape;
	float speed;
};

#endif