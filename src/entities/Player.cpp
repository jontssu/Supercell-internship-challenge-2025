#include "Player.h"
#include "ResourceManager.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>
#include <iostream>
#include "Constants.h"
#include "Projectile.h"

Player::Player()
{
}

sf::Vector2f Player::getShootDirection() const
{
    sf::Vector2f direction = m_mousePosition - m_position;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    // Normalize the direction
    if (length > 0.0f)
        direction /= length;
    else
        direction = sf::Vector2f(1.0f, 0.0f); // Default to right if mouse is on player
    
    return direction;
}

void Player::shoot(float dt, int type)
{
    if (m_shootCooldown > 0.0f)
        m_shootCooldown -= dt;
        
    if (m_shootCooldown <= 0.0f)
    {
        sf::Vector2f direction = getShootDirection();
        float projectileSpeed = ProjectileSpeed; // Pixels per second
        
        m_projectileRequest.position = m_position;
        m_projectileRequest.velocity = direction * projectileSpeed;
        m_projectileRequest.projectileType = type;  // Set the type!
        m_hasProjectileRequest = true;
        
        m_shootCooldown = m_attackSpeed;
    }
}

bool Player::init()
{
    const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("player.png");
    if (pTexture == nullptr)
        return false;

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
        return false;

    m_rotation = sf::degrees(0);
    sf::FloatRect localBounds = m_pSprite->getLocalBounds();
    m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
    m_pSprite->setPosition(m_position);
    m_pSprite->setScale(sf::Vector2f(3.0f, 3.0f));
    m_collisionRadius = collisionRadius;

    return true;
}

void Player::update(float dt)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        m_position.y -= 150 * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        m_position.y += 150 * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        m_position.x -= 150 * dt;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        m_position.x += 150 * dt;
    }

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        shoot(dt, PROJECTILE_TYPE_WATER);
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        shoot(dt, PROJECTILE_TYPE_FIRE);
}

void Player::render(sf::RenderTarget& target) const
{
    if (const sf::RenderWindow* window = dynamic_cast<const sf::RenderWindow*>(&target))
        m_mousePosition = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    
    m_pSprite->setRotation(m_rotation);
    m_pSprite->setPosition(m_position);
    target.draw(*m_pSprite);
}
