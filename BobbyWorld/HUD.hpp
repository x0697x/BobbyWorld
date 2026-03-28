#ifndef HUD_HPP
#define HUD_HPP

#include <SFML/Graphics.hpp>
#include <string>

class HUD {
public:
	HUD();
	// Update numbers
	void update(float dt, float playerRadius, int botCount);
	// Draw window (static)
	void draw(sf::RenderWindow& window);

private:
	sf::Font font;
	sf::Text massText;
	sf::Text botText;
	sf::Text fpsText;

	float fpsTimer;
	int frameCount;
	int lastFPS;
};

#endif