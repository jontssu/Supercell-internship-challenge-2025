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
        {
            m_enemies.push_back(std::move(pEnemy));
            std::cout << "Enemy spawned! Total enemies: " << m_enemies.size() << std::endl;
        }
        else
        {
            std::cout << "Enemy init failed!" << std::endl;
        }
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
        std::cout << "Creating projectile at (" << request.position.x << ", " << request.position.y << ")" << std::endl;
        m_projectiles.push_back(std::make_unique<Projectile>(request.position, request.velocity));
        m_pPlayer->clearProjectileRequest();
        std::cout << "Total projectiles: " << m_projectiles.size() << std::endl;
    }

    // Update projectiles
    for (auto& projectile : m_projectiles)
    {
        projectile->update(dt);
    }

    // Remove off-screen projectiles
    m_projectiles.erase(
        std::remove_if(m_projectiles.begin(), m_projectiles.end(),
            [](const std::unique_ptr<Projectile>& p) { return p->isOffScreen(); }),
        m_projectiles.end()
    );

    if (m_pParticleWorld)
        m_pParticleWorld->update(dt);

    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        pEnemy->update(dt);
    }

    // Detect collisions
    bool playerDied = false;
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        float distance = (m_pPlayer->getPosition() - pEnemy->getPosition()).lengthSquared();
        float minDistance = std::pow(Player::collisionRadius + pEnemy->getCollisionRadius(), 2.0f);
        // const sf::Vector2f playerPosition = m_pPlayer->getPosition();

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
