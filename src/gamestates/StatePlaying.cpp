#include "StatePlaying.h"
#include "StatePaused.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include <memory>
#include <algorithm>
#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>
#include "../particles/ParticleWorld.h"

StatePlaying::StatePlaying(StateStack& stateStack)
    : m_stateStack(stateStack)
{
}

bool StatePlaying::init()
{
    m_ground.setSize({1024.0f, 256.0f});
    m_ground.setPosition({0.0f, 800.0f});
    m_ground.setFillColor(sf::Color::Green);

    m_pParticleWorld = std::make_unique<ParticleWorld>();
    if (!m_pParticleWorld)
        return false;

    m_pPlayer = std::make_unique<Player>();
    if (!m_pPlayer || !m_pPlayer->init())
        return false;
    m_pPlayer->setPosition(sf::Vector2f(200, GroundLevel));
    m_pPlayer->setParticleWorld(m_pParticleWorld.get());


    return true;
}

void StatePlaying::update(float dt)
{
    // Enemy spawning
    m_timeUntilEnemySpawn -= dt;

    if (m_timeUntilEnemySpawn < 0.0f)
    {
        m_timeUntilEnemySpawn = enemySpawnInterval;
        std::unique_ptr<Enemy> pEnemy = std::make_unique<Enemy>();
        pEnemy->setPosition(sf::Vector2f(WindowWidth - 20, GroundLevel));
        if (pEnemy->init())
            m_enemies.push_back(std::move(pEnemy));
    }

    // Pause game
    bool isPauseKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
    m_hasPauseKeyBeenReleased |= !isPauseKeyPressed;
    if (m_hasPauseKeyBeenReleased && isPauseKeyPressed)
    {
        m_hasPauseKeyBeenReleased = false;
        m_stateStack.push<StatePaused>();
    }

    if (m_pPlayer)
        m_pPlayer->update(dt);

    // Spawn projectiles from player
    if (m_pPlayer && m_pPlayer->hasProjectileRequest())
    {
        auto request = m_pPlayer->getProjectileRequest();
        m_projectiles.push_back(std::make_unique<Projectile>(
            request.position, 
            request.velocity, 
            request.projectileType
        ));
        m_pPlayer->clearProjectileRequest();
    }

    // Update projectiles
    for (auto& projectile : m_projectiles)
        projectile->update(dt);

    // Remove off-screen projectiles
    for (int i = m_projectiles.size() - 1; i >= 0; --i)
    {
        if (m_projectiles[i]->isOffScreen())
            m_projectiles.erase(m_projectiles.begin() + i);
    }

    if (m_pParticleWorld)
        m_pParticleWorld->update(dt);

    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
        pEnemy->update(dt);

    // Check for bullet-enemy collisions
    for (int i = m_projectiles.size() - 1; i >= 0; --i)
    {
        for (int j = m_enemies.size() - 1; j >= 0; --j)
        {
            float distance = (m_projectiles[i]->getPosition() - m_enemies[j]->getPosition()).lengthSquared();
            float minDistance = std::pow(ProjectileWidth / 2.0f + m_enemies[j]->getCollisionRadius(), 2.0f);

            if (distance <= minDistance)
            {
                // Damage the enemy if type matches
                if (m_enemies[j]->setHealth(m_pPlayer->getDamage(), m_projectiles[i]->getProjectileType()))
                    m_enemies.erase(m_enemies.begin() + j);
                
                m_projectiles.erase(m_projectiles.begin() + i);
                break;
            }
        }
    }

    // Check for player-enemy collisions
    bool playerDied = false;
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        float distance = (m_pPlayer->getPosition() - pEnemy->getPosition()).lengthSquared();
        float minDistance = std::pow(Player::collisionRadius + pEnemy->getCollisionRadius(), 2.0f);

        if (distance <= minDistance)
        {
            playerDied = true;
            break;
        }
    }

    // End Playing State on player death
    if (playerDied)
        m_stateStack.popDeferred();
}

void StatePlaying::render(sf::RenderTarget& target) const
{
    // target.draw(m_ground);

    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
        pEnemy->render(target);
    
    for (const std::unique_ptr<Projectile>& projectile : m_projectiles)
        projectile->render(target);
    
    if (m_pPlayer)
        m_pPlayer->render(target);
    
    if (m_pParticleWorld)
        m_pParticleWorld->render(target);
}
