#ifndef HUD_HPP
#define HUD_HPP

#include <SFML/Graphics.hpp>
#include <string>

class HUD {
public:
	HUD();
	// Update
	void update(float dt, float playerRadius, int botCount, int eatenCount, float totalTime);

	// Draw
	void draw(sf::RenderWindow& window);

	const sf::Font& getFont() const { return font; }

private:
	sf::Font font;
	sf::Text massText;
	sf::Text botText;
	sf::Text fpsText;
	sf::Text eatenText;
	sf::Text timeText;
	sf::Text versionText;
	sf::Text creditText;
	sf::Text inputText;

	float fpsTimer;
	float totalTime;
	int frameCount;
	int lastFPS;
};

#endif