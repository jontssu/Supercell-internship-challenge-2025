#pragma once

#include "IState.h"
#include "entities/Player.h"
#include "entities/Enemy.h"
#include "entities/Projectile.h"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class StatePlaying : public IState
{
public:
    StatePlaying(StateStack& stateStack);
    ~StatePlaying() = default;

    bool init() override;
    void update(float dt) override;
	void renderScore(sf::RenderTarget &target) const;
	void render(sf::RenderTarget &target) const override;

private:
    float enemySpawnInterval = EnemySpawnInterval;
    float m_timeUntilEnemySpawn = enemySpawnInterval;
    float m_woodSpawnInterval = 2.5f;

    StateStack& m_stateStack;
    std::unique_ptr<Player> m_pPlayer;
    std::unique_ptr<ParticleWorld> m_pParticleWorld;
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::vector<std::unique_ptr<Projectile>> m_projectiles;
    sf::RectangleShape m_ground;
    const sf::Font* m_font = nullptr;
    unsigned int m_score = 0;
    bool m_hasPauseKeyBeenReleased = true;
    float m_difficultyTimer = 0.0f;
    float m_gameTime = 0.0f;
    unsigned int m_difficultyStage = 0; 
    unsigned int m_enemySpawnCount = EnemySpawnCount;

    void updateCollisions();
};
