#include "StateMenu.h"
#include "StatePlaying.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <fstream>
#include <iostream>

StateMenu::StateMenu(StateStack& stateStack)
    : m_stateStack(stateStack)
{
    
}

bool StateMenu::init()
{
    const sf::Font* pFont = ResourceManager::getOrLoadFont("Lavigne.ttf");
    if (pFont == nullptr)
        return false;

    // Load high score
    m_highScore = loadHighScore();

    m_pText = std::make_unique<sf::Text>(*pFont);
    if (!m_pText)
        return false;

    m_pText->setString("IN A WORLD FULL OF TRASH\nWE MUST SURVIVE\nTHERE IS NO CHOICE\n\n\nW A S D TO MOVE\nMOUSE TO AIM \nLEFT CLICK TO SHOOT FIRE\nRIGHT CLICK TO SHOOT ICE\nDESTROY ENEMIES WITH THE CORRECT ELEMENT TO SCORE POINTS\n\nAND ALSO... TRY TO AVOID TRASH :)\n\n\nPRESS <ENTER> TO START");
    m_pText->setStyle(sf::Text::Bold);
    sf::FloatRect localBounds = m_pText->getLocalBounds();
    m_pText->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});

    // Create high score text
    m_pHighScoreText = std::make_unique<sf::Text>(*pFont);
    if (!m_pHighScoreText)
        return false;

    m_pHighScoreText->setString("HIGH SCORE: " + std::to_string(m_highScore));
    m_pHighScoreText->setStyle(sf::Text::Bold);
    m_pHighScoreText->setCharacterSize(30);

    return true;
}

void StateMenu::update(float dt)
{
    (void)dt;
    
    // Reload high score every frame to ensure it's up-to-date
    m_highScore = loadHighScore();
    if (m_pHighScoreText)
        m_pHighScoreText->setString("HIGH SCORE: " + std::to_string(m_highScore));
    
    m_hasStartKeyBeenPressed |= sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter);
    if (m_hasStartKeyBeenReleased)
    {
        m_hasStartKeyBeenPressed = false;
        m_hasStartKeyBeenReleased = false;
        m_stateStack.push<StatePlaying>();
    }
    m_hasStartKeyBeenReleased |= m_hasStartKeyBeenPressed && !sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter);
}

void StateMenu::render(sf::RenderTarget& target) const
{
    m_pText->setPosition({target.getSize().x * 0.5f, target.getSize().y * 0.5f});
    target.draw(*m_pText);
    
    m_pHighScoreText->setPosition({WindowWidth - 275.0f, 10.0f});
    target.draw(*m_pHighScoreText);
}

unsigned int StateMenu::loadHighScore()
{
    std::ifstream file("highscore.txt");
    if (file.is_open())
    {
        unsigned int score = 0;
        file >> score;
        file.close();
        return score;
    }
    return 0;
}

void StateMenu::saveHighScore(unsigned int score)
{
    unsigned int currentHighScore = loadHighScore();
    if (score > currentHighScore)
    {
        std::ofstream file("highscore.txt");
        if (file.is_open())
        {
            file << score;
            file.close();
        }
    }
}
