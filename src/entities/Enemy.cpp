#include "Enemy.h"
#include "ResourceManager.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Constants.h"

bool Enemy::init()
{
    int enemyType = rand() % 7;
    if (enemyType == 0)
        m_type = ENEMY_TYPE_WATER;
    else if (enemyType == 1)
        m_type = ENEMY_TYPE_FIRE;
    else
        m_type = ENEMY_TYPE_WALL;

    const sf::Texture* pTexture = nullptr;
    if (m_type == ENEMY_TYPE_WATER)
        pTexture = ResourceManager::getOrLoadTexture("ice.png");
    else if (m_type == ENEMY_TYPE_FIRE)
        pTexture = ResourceManager::getOrLoadTexture("fire.png");
    else
        pTexture = ResourceManager::getOrLoadTexture("enemy.png");

    if (pTexture == nullptr)
    {
        std::cout << "ERROR: Failed to load enemy texture!" << std::endl;
        return false;
    }

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
    {
        std::cout << "ERROR: Failed to create enemy sprite!" << std::endl;
        return false;
    }

    m_pSprite->setTextureRect(sf::IntRect({0, 0}, {16, 16}));

    sf::FloatRect localBounds = m_pSprite->getLocalBounds();
    m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
    m_pSprite->setPosition(m_position);
    m_pSprite->setScale(sf::Vector2f(2.5f, 2.5f));
    m_collisionRadius = collisionRadius;

    return true;
}

void Enemy::update(float dt)
{
    m_position.x -= m_speed * dt;
    std::cout << "My hp is " << m_health << std::endl;
}

void Enemy::render(sf::RenderTarget& target) const
{
    m_pSprite->setPosition(m_position);
    target.draw(*m_pSprite);
}
