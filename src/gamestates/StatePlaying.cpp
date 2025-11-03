#include "StatePlaying.h"
#include "StatePaused.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include <memory>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <SFML/Graphics/RenderTarget.hpp>
#include "../particles/ParticleWorld.h"
#include "../particles/Particle.h"
#include "../Constants.h"

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
    m_pPlayer->setParticleWorldPointer(m_pParticleWorld.get());

    m_font = ResourceManager::getOrLoadFont("Lavigne.ttf");
    if (!m_font)
        return false;

    m_score = 0;
    return true;
}

void StatePlaying::update(float dt)
{
    // Track total game time
    m_gameTime += dt;
    
    // Difficulty scaling
    m_difficultyTimer += dt;
    if (m_difficultyTimer >= 10.0f)
    {
        m_difficultyTimer = 0.0f;
        enemySpawnInterval = std::max(0.1f, enemySpawnInterval - 0.1f);
        m_difficultyStage += 1;
        if (m_difficultyStage % 5 == 0)
            m_enemySpawnCount += 1;
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
    
    // Pass game time to player for particle push scaling
    if (m_pPlayer)
        m_pPlayer->setGameTime(m_gameTime);

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

    // Spawn enemies
    m_timeUntilEnemySpawn -= dt;
    if (m_timeUntilEnemySpawn <= 0.0f)
    {
        m_timeUntilEnemySpawn = enemySpawnInterval;
        for (unsigned int i = 0; i < m_enemySpawnCount; ++i)
        {
            auto pEnemy = std::make_unique<Enemy>();
            if (pEnemy && pEnemy->init())
            {
                float randomX = static_cast<float>(rand() % static_cast<int>(WindowWidth));
                float randomY = static_cast<float>(rand() % static_cast<int>(WindowHeight / 2));
                pEnemy->setPosition(sf::Vector2f(randomX, randomY));
                pEnemy->setSpeed(EnemySpeed);
                m_enemies.push_back(std::move(pEnemy));
            }
        }
    }

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
                {
                    m_enemies.erase(m_enemies.begin() + j);
                    m_score += 10.0f;
                }
                
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

    // Particle spawner - alternates between material types
    static float particleSpawnTimer = 0.0f;
    static float materialSwitchTimer = 0.0f;
    static float materialSwitchDuration = 1.0f; // Initial duration
    static int currentMaterialType = MAT_ID_SAND;
    
    particleSpawnTimer += dt;
    materialSwitchTimer += dt;
    
    // Switch material type after duration expires
    if (materialSwitchTimer >= materialSwitchDuration)
    {
        materialSwitchTimer = 0.0f;
        
        // Toggle between SAND and WATER
        currentMaterialType = (currentMaterialType == MAT_ID_SAND) ? MAT_ID_WATER : MAT_ID_SAND;
        
        // Sand periods last much longer than water periods
        if (currentMaterialType == MAT_ID_SAND)
        {
            // Sand: 2.0 to 4.0 seconds
            materialSwitchDuration = 2.0f + static_cast<float>(rand() % 201) / 100.0f;
        }
        else
        {
            // Water: 0.2 to 1 seconds
            materialSwitchDuration = 0.2f + static_cast<float>(rand() % 81) / 100.0f;
        }
    }
    
    if (m_pParticleWorld && particleSpawnTimer > 0.001f)
    {
        particleSpawnTimer = 0.0f;

        for (int i = 0; i < 1; ++i)
        {
            float randomY = static_cast<float>(rand() % static_cast<int>(WindowHeight));
            float randomX = WindowWidth - 10.0f - static_cast<float>(rand() % 40);
            sf::Vector2f spawnPosition(randomX, 10);
            sf::Vector2f velocity(0.0f, 0.0f);
            
            m_pParticleWorld->addParticle(spawnPosition, velocity, currentMaterialType);
        }
    }

}

void StatePlaying::renderScore(sf::RenderTarget& target) const
{
    if (!m_font)
        return;
    
    sf::Text scoreText(*m_font);
    scoreText.setString("Score: " + std::to_string(m_score));
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition({10.f, 10.f});
    target.draw(scoreText);
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

    renderScore(target);
}
