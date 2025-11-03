#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace sf { class RenderTarget; }

class Projectile
{
public:
    Projectile(const sf::Vector2f& position, const sf::Vector2f& velocity);
    
    void update(float dt);
    void render(sf::RenderTarget& target) const;
    
    const sf::Vector2f& getPosition() const { return m_position; }
    bool isOffScreen() const;
    
private:
	sf::RectangleShape m_rectangle;
    sf::Vector2f m_position;
    sf::Vector2f m_velocity;
};
