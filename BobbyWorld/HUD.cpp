#include "HUD.hpp"
#include <iomanip>
#include <sstream>

HUD::HUD() :
	fpsTimer(0.f),
	frameCount(0),
	lastFPS(0),
	// Init text obj with font
	massText(font),
	botText(font),
	fpsText(font)
{
	// Load font
	if (!font.openFromFile("Resources/bobbyfont.ttf")) {
		// Handle error
	}

	// FPS
	fpsText.setFont(font);
	fpsText.setCharacterSize(18);
	fpsText.setFillColor(sf::Color::White);
	fpsText.setPosition({ 20.f, 10.f });

	// Mass
	massText.setFont(font);
	massText.setCharacterSize(18);
	massText.setFillColor(sf::Color::White);
	massText.setPosition({ 20.f, 35.f });

	// Bot count
	botText.setFont(font);
	botText.setCharacterSize(18);
	botText.setFillColor(sf::Color::White);
	botText.setPosition({ 20.f, 65.f });
}

void HUD::update(float dt, float playerRadius, int botCount) {
	// Update mass
	std::stringstream ss;
	ss << "Mass: " << std::fixed << std::setprecision(0) << (playerRadius * playerRadius / 10.f);
	massText.setString(ss.str());

	// Update bot
	botText.setString("Bots in world: " + std::to_string(botCount));

	// FPS
	fpsTimer += dt;
	frameCount++;
	if (fpsTimer >= 1.0f) {
		lastFPS = frameCount;
		frameCount = 0;
		fpsTimer -= 1.0f;
		fpsText.setString("FPS: " + std::to_string(lastFPS));
	}
}

void HUD::draw(sf::RenderWindow& window) {
	// Important: switch to default UI view so it stay fixed
	sf::View oldView = window.getView();
		window.setView(window.getDefaultView());

		window.draw(massText);
		window.draw(botText);
		window.draw(fpsText);

		// Sitch back to game cam
		window.setView(oldView);
}