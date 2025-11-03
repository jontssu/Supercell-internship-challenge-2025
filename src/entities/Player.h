#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "particles/ParticleWorld.h"
#include "Constants.h"

namespace sf { class Sprite; }

class Player final : public Entity
{
public:
    static constexpr float collisionRadius = 42.0f;
    
    struct ProjectileRequest {
        sf::Vector2f position;
        sf::Vector2f velocity;
        int projectileType;  // Which type of projectile
    };

    Player();
	virtual ~Player() = default;

	void initPhysics();

	void setParticleWorld(ParticleWorld* pParticleWorld) { m_pParticleWorld = std::make_unique<ParticleWorld>(*pParticleWorld); }

    void setParticleWorldPointer(ParticleWorld* pParticleWorld) { m_pParticleWorldPtr = pParticleWorld; }

    void setGameTime(float gameTime) { m_gameTime = gameTime; }

    ProjectileRequest getProjectileRequest() const { return m_projectileRequest; }
    const float getDamage() const { return m_damage; }

    bool hasProjectileRequest() const { return m_hasProjectileRequest; }
    void clearProjectileRequest() { m_hasProjectileRequest = false; }

    void shoot(float dt, int type);

    bool init() override;
	void updatePhysics(float dt);
	void update(float dt) override;
	void render(sf::RenderTarget& target) const override;

    bool m_isJumping = false;

private:
    std::unique_ptr<ParticleWorld> m_pParticleWorld;
    ParticleWorld* m_pParticleWorldPtr = nullptr;
    float m_shootCooldown = 0.0f;
    float m_attackSpeed = AttackSpeed;
    float m_damage = PlayerDamage;
    bool m_hasProjectileRequest = false;
    ProjectileRequest m_projectileRequest;
    mutable sf::Vector2f m_mousePosition;
    sf::Vector2f m_velocity = {0.0f, 0.0f};
    bool m_inWater = false;
    float m_groundLevel = GroundLevel;
    float m_gameTime = 0.0f;


    //Physics
    sf::Vector2f velocity;
    float velocityMax;
    float velocityMin;
    float acceleration;
    float drag;
    float gravity;
    float velocityMaxY;

    sf::Vector2f getShootDirection() const;
};
