#include "Player.hpp"

Player::Player(float radius, sf::Color color) : speed(300.0f) {
	shape.setRadius(radius);
	shape.setFillColor(color);
	shape.setOrigin({ radius, radius });
	shape.setPosition({ 600.0f, 400.0f });
}

void Player::update(float dt) {
	sf::Vector2f movement(0.f, 0.f);

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z)) movement.y -= 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) movement.y += 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) movement.x -= 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) movement.x += 1.f;

	// Prevent diagonal is faster
	if (movement.x != 0.f || movement.y != 0.f) {
		float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
		movement /= length;
	}

	// Move shape
	shape.move(movement * speed * dt);
}

void Player::draw(sf::RenderWindow& window) {
	window.draw(shape);
}

void Player::grow(float amount) {
	float newRadius = shape.getRadius() + amount;
	shape.setRadius(newRadius);
	shape.setOrigin({ newRadius, newRadius }); //Recenter after grow
}