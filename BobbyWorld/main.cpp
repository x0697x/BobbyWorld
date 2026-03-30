#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cmath>
#include "Player.hpp"
#include "Bot.hpp"
#include "HUD.hpp"
#include "GameManager.hpp"
#include "DiscordManager.hpp"

int main() {
    // Setting antialias
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    // Window
    sf::RenderWindow window(sf::VideoMode(sf::Vector2u(1280, 720)), "BobbyWorld", sf::State::Windowed, settings);
    window.setFramerateLimit(60);

    // Background
    std::vector<sf::CircleShape> stars;
    std::mt19937 gen(1337);
    std::uniform_real_distribution<float> distX(0.0f, 1.0f);
    std::uniform_real_distribution<float> distY(0.0f, 1.0f);

    for (int i = 0; i < 2000; ++i) {
        sf::CircleShape star(1.5f);
        star.setFillColor(sf::Color(150, 150, 150));
        star.setPosition({ distX(gen), distY(gen) });
        stars.push_back(star);
    }

    Player bobby(20.0f, sf::Color::White);
    sf::View view(sf::FloatRect({ 0, 0 }, { 1280, 800 }));

    // HUD & MANAGERS
    HUD hud;
    GameManager gm;
    DiscordManager discordRPC;
    bool isDead = false;

    // Initial bots spawn
    std::vector<Bot> bots;
    for (int i = 0; i < 100; ++i) {
        std::uniform_real_distribution<float> posDist(-2000.f, 2000.f); // Wider range
        std::uniform_real_distribution<float> sizeDist(10.f, 40.f);

        sf::Vector2f spawnPos;
        float minSafeDistance = 600.f; // Bot dist from Bobby at start

        // Keep gen pos until outside safezone
        do {
            spawnPos = { posDist(gen), posDist(gen) };
            float distToCenter = std::sqrt(spawnPos.x * spawnPos.x + spawnPos.y * spawnPos.y);
            if (distToCenter > minSafeDistance) break;
        } while (true);

        bots.emplace_back(sizeDist(gen), spawnPos, gen);
    }

    // ----DEATH----
    // Death screen overlay
    sf::RectangleShape deathOverlay(sf::Vector2f(1280.f, 720.f));
    deathOverlay.setFillColor(sf::Color(0, 0, 0, 150));

    sf::Text deathText(hud.getFont());
    deathText.setString("   YOU ARE DEAD\nPress ESC to exit");
    deathText.setCharacterSize(60);
    deathText.setFillColor(sf::Color::Red);
    deathText.setStyle(sf::Text::Bold);

    // Center
    sf::FloatRect textBounds = deathText.getLocalBounds();
    deathText.setPosition({ 550.f / 2.0f, 550.f / 2.0f });
    // ----DEATH----

    // Load textures
    sf::Texture tex1, tex2, tex3, tex4, tex5, tex6, tex7, tex8;
    tex1.loadFromFile("Resources/tex1.jpg");
    tex2.loadFromFile("Resources/tex2.jpg");
    tex3.loadFromFile("Resources/tex3.jpg");
    tex4.loadFromFile("Resources/tex4.jpg");
    tex5.loadFromFile("Resources/tex5.jpg");
    tex6.loadFromFile("Resources/tex6.jpg");
    tex7.loadFromFile("Resources/tex7.jpg");
    tex8.loadFromFile("Resources/tex8.jpg");

    sf::Clock clock;
    float rpcTimer = 0.f;
    
    //                                                                                                                                                
    //     mmm  mmm     mm      mmmmmm   mmm   mm               mmmm      mm     mmm  mmm  mmmmmmmm            mm          mmmm      mmmm    mmmmmm   
    //     ###  ###    ####     ""##""   ###   ##             ##""""#    ####    ###  ###  ##""""""            ##         ##""##    ##""##   ##""""#m 
    //     ########    ####       ##     ##"#  ##            ##          ####    ########  ##                  ##        ##    ##  ##    ##  ##    ## 
    //     ## ## ##   ##  ##      ##     ## ## ##            ##  mmmm   ##  ##   ## ## ##  #######             ##        ##    ##  ##    ##  ######"  
    //     ## "" ##   ######      ##     ##  #m##            ##  ""##   ######   ## "" ##  ##                  ##        ##    ##  ##    ##  ##       
    //     ##    ##  m##  ##m   mm##mm   ##   ###             ##mmm##  m##  ##m  ##    ##  ##mmmmmm            ##mmmmmm   ##mm##    ##mm##   ##       
    //     ""    ""  ""    ""   """"""   ""   """               """"   ""    ""  ""    ""  """"""""            """"""""    """"      """"    ""       
    //                                                                                                                                                
    //                                                                                                                                                

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        // ----INPUT & EVENTS----
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Check key press
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                // Exit
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }

        if (!isDead) {
            if (window.hasFocus()) {
                bobby.update(dt);
            }
        }

        float zoomFactor = 1.0f + (bobby.getRadius() - 20.0f) / 200.0f;

        // Bots logic
        for (int i = 0; i < bots.size(); ++i) {
            sf::Vector2f dToB = bobby.getPosition() - bots[i].getPosition();
            float distSq = dToB.x * dToB.x + dToB.y * dToB.y;
            float maxVisibleDist = 3500.f * zoomFactor;

            if (distSq > maxVisibleDist * maxVisibleDist) {
                bots.erase(bots.begin() + i);
                --i;
                continue;
            }

            bots[i].update(dt, bobby.getPosition(), gen);

            float distance = std::sqrt(distSq);
            if (distance < bobby.getRadius() + bots[i].getRadius()) {
                if (bobby.getRadius() > bots[i].getRadius()) {
                    bobby.grow(bots[i].getRadius() * 0.15f);
                    bots.erase(bots.begin() + i);
                    gm.addEaten();
                    --i;
                    continue;
                }
                else {
                    isDead = true; // Game over
                }
            }

            for (int j = i + 1; j < bots.size(); ++j) {
                sf::Vector2f bDiff = bots[i].getPosition() - bots[j].getPosition();
                float bDistSq = bDiff.x * bDiff.x + bDiff.y * bDiff.y;
                float combinedR = bots[i].getRadius() + bots[j].getRadius();

                if (bDistSq < combinedR * combinedR) {
                    if (bots[i].getRadius() > bots[j].getRadius()) {
                        if (bots[i].getRadius() < bobby.getRadius() * 2.0f) {
                            bots[i].setRadius(bots[i].getRadius() + bots[j].getRadius() * 0.1f);
                        }
                        bots.erase(bots.begin() + j);
                        --j;
                    }
                    else {
                        if (bots[j].getRadius() < bobby.getRadius() * 2.0f) {
                            bots[j].setRadius(bots[j].getRadius() + bots[i].getRadius() * 0.1f);
                        }
                        bots.erase(bots.begin() + i);
                        --i;
                        break;
                    }
                }
            }
        }

        // Spawn                                                                                

        if (bots.size() < 50) {
            float minSpawnDist = 1500.f * zoomFactor;
            float maxSpawnDist = 3000.f * zoomFactor;
            std::uniform_real_distribution<float> angleDist(0, 2.f * 3.14159f);
            std::uniform_real_distribution<float> distDist(minSpawnDist, maxSpawnDist);

            float angle = angleDist(gen);
            float radius = distDist(gen);
            sf::Vector2f spawnPos = bobby.getPosition() + sf::Vector2f(cos(angle) * radius, sin(angle) * radius);

            float minS = std::max(10.f, bobby.getRadius() * 0.4f);
            float maxS = std::max(40.f, bobby.getRadius() * 2.5f);
            std::uniform_real_distribution<float> sizeDist(minS, maxS);
            float finalSize = sizeDist(gen);

            // Check if spawn is Alpha (10% chance)
            if (std::uniform_real_distribution<float>(0, 1)(gen) > 0.9f) {
                finalSize *= 1.5f;
                bots.emplace_back(finalSize, spawnPos, gen);
                bots.back().setAlpha(true); // Flag aggressive
                //bots.back().setColor(sf::Color::Red); // Alpha color
            }
            else {
                bots.emplace_back(finalSize, spawnPos, gen);
            }
        }

        // Updates                                                                                                                                                                                                                    

        gm.update(dt, bobby, tex1, tex2, tex3, tex4, tex5, tex6, tex7, tex8);
        hud.update(dt, bobby.getRadius(), static_cast<int>(bots.size()), gm.getEatenCount(), gm.getTotalTime());
        discordRPC.runCallbacks();

        rpcTimer += dt;
        if (rpcTimer >= 5.0f) {
            float currentMass = (bobby.getRadius() * bobby.getRadius()) / 10.f;
            discordRPC.update(currentMass, gm.getEatenCount());
            rpcTimer = 0.f;
        }

        //                                                                                              
        //     mmmmmm    mmmmmmmm  mmm   mm  mmmmm     mmmmmmmm  mmmmmm     mmmmmm   mmm   mm     mmmm  
        //     ##""""##  ##""""""  ###   ##  ##"""##   ##""""""  ##""""##   ""##""   ###   ##   ##""""# 
        //     ##    ##  ##        ##"#  ##  ##    ##  ##        ##    ##     ##     ##"#  ##  ##       
        //     #######   #######   ## ## ##  ##    ##  #######   #######      ##     ## ## ##  ##  mmmm 
        //     ##  "##m  ##        ##  #m##  ##    ##  ##        ##  "##m     ##     ##  #m##  ##  ""## 
        //     ##    ##  ##mmmmmm  ##   ###  ##mmm##   ##mmmmmm  ##    ##   mm##mm   ##   ###   ##mmm## 
        //     ""    """ """"""""  ""   """  """""     """"""""  ""    """  """"""   ""   """     """"  
        //                                                                                             

        view.setSize({ 1280 * zoomFactor, 720 * zoomFactor });
        view.setCenter(bobby.getPosition());

        window.clear(sf::Color::Black);
        window.setView(view);

        // Infinite background logic
        float baseWorldSize = 4000.0f;
        float dynamicWorldSize = baseWorldSize * zoomFactor;

        for (const auto& star : stars) {
            sf::Vector2f relativePos = star.getPosition();
            sf::Vector2f absoluteStarPos = { relativePos.x * dynamicWorldSize, relativePos.y * dynamicWorldSize };

            sf::Vector2f vPos = view.getCenter();

            // Tiling math
            float x = std::fmod(absoluteStarPos.x - vPos.x + dynamicWorldSize / 2.0f, dynamicWorldSize);
            if (x < 0) x += dynamicWorldSize;

            float y = std::fmod(absoluteStarPos.y - vPos.y + dynamicWorldSize / 2.0f, dynamicWorldSize);
            if (y < 0) y += dynamicWorldSize;

            sf::CircleShape ts = star;
            ts.setRadius(1.5f * (zoomFactor * 0.5f + 0.5f));
            ts.setPosition({ x + vPos.x - dynamicWorldSize / 2.0f, y + vPos.y - dynamicWorldSize / 2.0f });
            window.draw(ts);
        }

        for (auto& bot : bots) bot.draw(window);
        bobby.draw(window);
        gm.drawSparkles(window);

        window.setView(window.getDefaultView());
        hud.draw(window);

        if (isDead) {
            window.setView(window.getDefaultView());
            window.draw(deathOverlay);
            window.draw(deathText);
        }

        window.display();
    }

    return 0;
}