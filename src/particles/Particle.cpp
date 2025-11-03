#include "Particle.h"

Particle::Particle(int _id, float _lifetime, sf::Vector2f _velocity, sf::Color _color)
	: id(_id), lifetime(_lifetime), velocity(_velocity), color(_color), has_been_updated(false)
{
	dispersityRate = 4;
	if (id == MAT_ID_WOOD || id == MAT_ID_OIL)
		isFlammable = true;
	if (id == MAT_ID_WOOD)
		lifetime = MAT_WOOD_LIFETIME;
	if (id == MAT_ID_FIRE)
		lifetime = MAT_FIRE_LIFETIME;
}

bool Particle::burn(float dt) {
	lifetime -= dt;
	if (lifetime <= 0) {
		return true;
	}
	return false;
}

void Particle::update()
{
}