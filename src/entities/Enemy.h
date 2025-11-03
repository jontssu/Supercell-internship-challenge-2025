#pragma once

#include "Entity.h"
#include <memory>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace sf { class Sprite; }

enum EnemyType
{
    ENEMY_TYPE_WATER,
    ENEMY_TYPE_FIRE,
    ENEMY_TYPE_FROSTFIRE
};

class Enemy final : public Entity
{
public:
    static constexpr float collisionRadius = 24.0f;

    inline bool setHealth(int damage, int projectileType) { if (projectileType == m_type) m_health -= damage; return isDead(); }
    inline const int getType() const { return m_type; }
    inline const bool isDead() const { return m_health <= 0; }

    Enemy() = default;
    virtual ~Enemy() = default;
    
    bool init() override;
    void update(float dt) override;
    void render(sf::RenderTarget& target) const override;

private:
    int m_type;
    int m_health = 100;
};
