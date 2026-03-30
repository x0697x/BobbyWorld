#include "Bot.hpp"

Bot::Bot(float radius, sf::Vector2f position, std::mt19937& gen) {
	shape.setRadius(radius);
	shape.setOrigin({ radius, radius });
	shape.setPosition(position);
	shape.setFillColor(sf::Color::White); // Default color

	speed = 100.0f;
	changeDirTimer = 0.0f;

	// Initial random direction
	std::uniform_real_distribution<float> angleDist(0, 2.0f * 3.14159f);
	float angle = angleDist(gen);
	direction = { cos(angle), sin(angle) };
}

void Bot::update(float dt, sf::Vector2f playerPos, std::mt19937& gen) {
	// Calculate distance to Bobby
	sf::Vector2f toPlayer = playerPos - shape.getPosition();
	float distance = std::sqrt(toPlayer.x * toPlayer.x + toPlayer.y * toPlayer.y);

	// Chase
	float detectionRange = 600.0f;
	if (isAlpha && distance < detectionRange) {
		direction = toPlayer / distance; // Normalize
		speed = 150.0f; // Alpha faster when chase
	}
	else {
		changeDirTimer -= dt;

		if (changeDirTimer <= 0.0f) {
			// New random direction
			std::uniform_real_distribution<float> angleDist(0, 2.0f * 3.14159f);
			float angle = angleDist(gen);
			direction = { cos(angle), sin(angle) };

			// Reset timer to change direction ; "*,*" = between
			std::uniform_real_distribution<float> timerDist(1.0f, 3.0f);
			changeDirTimer = timerDist(gen);
		}
	}

	shape.move(direction * speed * dt);
}

void Bot::draw(sf::RenderWindow& window) {
	window.draw(shape);
}

void Bot::setRadius(float newRadius) {
	shape.setRadius(newRadius);
	shape.setOrigin({ newRadius, newRadius });
}

void Bot::setColor(sf::Color color) {
	shape.setFillColor(color);
}