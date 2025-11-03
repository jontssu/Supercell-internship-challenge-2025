#include "Player.h"
#include "ResourceManager.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include <iostream>
#include "Constants.h"
#include "Projectile.h"

Player::Player()
{
}

sf::Vector2f Player::getShootDirection() const
{
    // Shoot to the right
    return sf::Vector2f(1.0f, 0.0f);
}

void Player::shoot(float dt, int type)
{
    if (m_shootCooldown > 0.0f)
        m_shootCooldown -= dt;
        
    if (m_shootCooldown <= 0.0f)
    {
        sf::Vector2f direction = getShootDirection();
        float projectileSpeed = 600.0f; // Pixels per second
        
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
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        m_position.y += 150 * dt;
    }
    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
    // {
    //     if (m_position.y >= GroundLevel)
    //     m_isJumping = true;
    // }
    // if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    // {
    //     m_isJumping = false;
    //     if (m_position.y < GroundLevel)
    //     m_position.y += 1.0f;
    // }

    // if (m_position.y < GroundLevel - 100)
    //     m_isJumping = false;

    // if (m_isJumping)
    //     m_position.y -= 200 * dt;
    // else if (!m_isJumping && m_position.y < GroundLevel)
    //     m_position.y += 200 * dt;

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        shoot(dt, PROJECTILE_TYPE_WATER);
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        shoot(dt, PROJECTILE_TYPE_FIRE);
}

void Player::render(sf::RenderTarget& target) const
{
    m_pSprite->setRotation(m_rotation);
    m_pSprite->setPosition(m_position);
    target.draw(*m_pSprite);
}
