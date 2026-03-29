#include "HUD.hpp"
#include <iomanip>
#include <sstream>

HUD::HUD() :
	fpsTimer(0.f),
	frameCount(0),
	lastFPS(0),
	totalTime(0.f),
	// SFML 3 requires passing the font in the initializer list:
	massText(font),
	botText(font),
	fpsText(font),
	eatenText(font),
	timeText(font),
	versionText(font),
	creditText(font)

{
	// Load font
	if (!font.openFromFile("Resources/bobbyfont.ttf")) {
		// Handle error
	}

	// Helper lambda to apply the border to all texts easily
	auto applyStyle = [](sf::Text& t) {
		t.setOutlineColor(sf::Color::White);
		t.setOutlineThickness(1.f);
		// Optional: If the border is white, you might want the inner text 
		// to be a bit darker (like Grey or Black) so the border stands out.
		t.setFillColor(sf::Color::White);
		t.setOutlineColor(sf::Color::Black);
		};

	// Apply to all your HUD elements
	applyStyle(fpsText);
	applyStyle(timeText);
	applyStyle(eatenText);
	applyStyle(massText);
	applyStyle(botText);
	applyStyle(versionText);
	applyStyle(creditText);

	// FPS
	fpsText.setFont(font);
	fpsText.setCharacterSize(18);
	fpsText.setFillColor(sf::Color::White);
	fpsText.setPosition({ 20.f, 10.f });

	// Time
	timeText.setFont(font);
	timeText.setCharacterSize(18);
	timeText.setFillColor(sf::Color::White);
	timeText.setPosition({ 20.f, 35.f });

	totalTime = 0.f;

	// Bots eaten
	eatenText.setFont(font);
	eatenText.setCharacterSize(18);
	eatenText.setFillColor(sf::Color::White);
	eatenText.setPosition({ 20.f, 65.f });

	// Mass
	massText.setFont(font);
	massText.setCharacterSize(18);
	massText.setFillColor(sf::Color::White);
	massText.setPosition({ 20.f, 90.f });

	// Bot count
	botText.setFont(font);
	botText.setCharacterSize(18);
	botText.setFillColor(sf::Color::White);
	botText.setPosition({ 20.f, 115.f });

	// Version Text Setup
	versionText.setFont(font);
	versionText.setString("Version 0.5"); // Change * to your version
	versionText.setCharacterSize(16);
	versionText.setFillColor(sf::Color(200, 200, 200, 150)); // Slightly transparent grey
	// Position: Right side (1280 - margin) and Bottom (720 - margin)
	versionText.setPosition({ 1112.f, 660.f });

	// Credit Text Setup
	creditText.setFont(font);
	creditText.setString("Made by x0697x");
	creditText.setCharacterSize(16);
	creditText.setFillColor(sf::Color(200, 200, 200, 150));
	creditText.setPosition({ 1050.f, 680.f });
}

void HUD::update(float dt, float playerRadius, int botCount, int eatenCount, float totalTime) {
	// Time init
	int minutes = static_cast<int>(totalTime) / 60;
	int seconds = static_cast<int>(totalTime) % 60;

	// Eaten bots
	eatenText.setString("Eaten: " + std::to_string(eatenCount));

	// Time string
	std::stringstream timeSS;
	timeSS << "Session: " << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
	timeText.setString(timeSS.str());

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
		window.draw(timeText);
		window.draw(eatenText);
		window.draw(versionText);
		window.draw(creditText);

		// Sitch back to game cam
		window.setView(oldView);
}