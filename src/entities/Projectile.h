#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace sf { class RenderTarget; }

enum ProjectileType
{
	PROJECTILE_TYPE_WATER = 0,
	PROJECTILE_TYPE_FIRE = 1
};

class Projectile
{
public:
    Projectile(const sf::Vector2f& position, const sf::Vector2f& velocity, int projectileType);
    
    inline const sf::Vector2f& getPosition() const { return m_position; }
	inline const int getProjectileType() const { return m_projectileType; } 
    bool isOffScreen() const;

    void update(float dt);
    void render(sf::RenderTarget& target) const;
    
    
private:
	sf::RectangleShape m_rectangle;
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
	int m_projectileType;
};
