#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include "Constants.h"

namespace sf { class Sprite; }

enum EnemyType
{
    ENEMY_TYPE_WATER = 0,
    ENEMY_TYPE_FIRE = 1,
    ENEMY_TYPE_WALL = 2
};

class Enemy final : public Entity
{
public:
    static constexpr float collisionRadius = 8.0f;

    inline bool setHealth(int damage, int projectileType) { if (projectileType == m_type) m_health -= damage; return isDead(); }
    inline void setSpeed(float speed) { m_speed = speed; }
    inline void setDamage(int damage) { m_damage = damage; }
    
    inline const float getSpeed() const { return m_speed; }
    inline const int getHealth() const { return m_health; }
    inline const int getDamage() const { return m_damage; }
    inline const int getType() const { return m_type; }
    inline const bool isDead() const { return m_health <= 0; }
    inline const bool isExpired() const { return m_lifetime >= 5.0f; }

    Enemy() = default;
    virtual ~Enemy() = default;
    
    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

private:
    int m_type;
    int m_health = EnemyHealth;
    int m_damage = EnemyDamage;
    float m_speed = EnemySpeed;
    float m_lifetime = 0.0f;
};
