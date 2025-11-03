#include "Enemy.h"
#include "ResourceManager.h"
#include <cmath>
#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

bool Enemy::init()
{
    std::cout << "Enemy::init() called" << std::endl;
    const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("enemy.png");
    if (pTexture == nullptr)
    {
        std::cout << "ERROR: Failed to load enemy.png texture!" << std::endl;
        return false;
    }

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
    {
        std::cout << "ERROR: Failed to create enemy sprite!" << std::endl;
        return false;
    }

    sf::FloatRect localBounds = m_pSprite->getLocalBounds();
    m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
    m_pSprite->setPosition(m_position);
    m_pSprite->setScale(sf::Vector2f(2.5f, 2.5f));
    m_collisionRadius = collisionRadius;

    std::cout << "Enemy initialized at position (" << m_position.x << ", " << m_position.y << ")" << std::endl;
    return true;
}

void Enemy::update(float dt)
{
    m_position.x -= 200 * dt;
}

void Enemy::render(sf::RenderTarget& target) const
{
    m_pSprite->setPosition(m_position);
    target.draw(*m_pSprite);
}
