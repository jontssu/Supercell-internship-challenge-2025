#include "Projectile.h"
#include "Constants.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <iostream>

Projectile::Projectile(const sf::Vector2f& position, const sf::Vector2f& velocity)
{
	m_position = position;
	m_velocity = velocity;
	
	// Setup the rectangle
	m_rectangle.setSize(sf::Vector2f(ProjectileWidth, ProjectileHeight));
	m_rectangle.setFillColor(sf::Color::Yellow);
	m_rectangle.setOrigin(sf::Vector2f(ProjectileWidth / 2.0f, ProjectileHeight / 2.0f)); // Center it
	m_rectangle.setPosition(m_position);
}

void Projectile::update(float dt)
{
    m_position += m_velocity * dt;
	m_rectangle.setPosition(m_position);
}

void Projectile::render(sf::RenderTarget& target) const
{
    target.draw(m_rectangle);
}

bool Projectile::isOffScreen() const
{
    return m_position.x < -50 || m_position.x > WindowWidth + 50 ||
           m_position.y < -50 || m_position.y > WindowHeight + 50;
}
