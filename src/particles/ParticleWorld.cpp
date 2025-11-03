#include "ParticleWorld.h"
#include "Constants.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <iostream>

ParticleWorld::ParticleWorld()
{
	particles.resize(GRID_WIDTH);
	for (auto &col : particles)
		col.resize(GRID_HEIGHT);
}

Particle &ParticleWorld::getParticleAt(int x, int y)
{
	return particles[x][y];
}

void ParticleWorld::addParticle(const sf::Vector2f &position, sf::Vector2f velocity, int mat_id)
{
	int x = static_cast<int>(position.x) / ParticleScale;
	int y = static_cast<int>(position.y) / ParticleScale;
	if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT)
	{
		std::cout << "Attempted to add particle out of bounds at (" << x << ", " << y << ")\n";
		return;
	}

	particles[x][y] = Particle(mat_id, 5.f, velocity, sf::Color::White);
}

void ParticleWorld::updateSand(int x, int y)
{
	Particle& sand = getParticleAt(x, y);
	if (sand.HasBeenUpdated())
		return;
	sand.setHasBeenUpdated(true);

	if (y + 1 < GRID_HEIGHT)  // Check downward
	{
		// Try to fall as far as velocity allows
		int maxFallDistance = static_cast<int>(sand.getVelocity().y);
		if (maxFallDistance <= 0)
			maxFallDistance = 1;

		int fallDistance = 0;
		for (int i = 1; i <= maxFallDistance && y + i < GRID_HEIGHT; ++i)
		{
			Particle& below = getParticleAt(x, y + i);
			if (below.getId() == MAT_ID_EMPTY)
				fallDistance = i;
			else
				break;
		}

		// If we can fall straight down, do it
		if (fallDistance > 0)
		{
			std::swap(sand, getParticleAt(x, y + fallDistance));
			return;
		}

		Particle& below = getParticleAt(x, y + 1);
		if (below.getId() == MAT_ID_WATER)
		{
			std::swap(sand, below);
			return;
		}

		// Can't fall straight, try diagonal downward
		if (x - 1 >= 0)
		{
			Particle& belowLeft = getParticleAt(x - 1, y + 1);
			if (belowLeft.getId() == MAT_ID_EMPTY
				|| belowLeft.getId() == MAT_ID_WATER)
			{
				std::swap(sand, belowLeft);
				return;
			}
		}

		if (x + 1 < GRID_WIDTH)
		{
			Particle& belowRight = getParticleAt(x + 1, y + 1);
			if (belowRight.getId() == MAT_ID_EMPTY
				|| belowRight.getId() == MAT_ID_WATER)
			{
				std::swap(sand, belowRight);
				return;
			}
		}
	}
	
	// Move left at the end if no other movement occurred (only when timer allows)
	if (shouldMoveLeftThisFrame)
	{
		if (x > 0)
		{
			Particle& left = getParticleAt(x - 1, y);
			if (left.getId() == MAT_ID_EMPTY)
			{
				std::swap(sand, left);
				return;
			}
		}
		else if (x == 0)
		{
			// Delete particle at left edge
			sand.setId(MAT_ID_EMPTY);
		}
	}
}

void ParticleWorld::updateWater(int x, int y)
{
	Particle& water = getParticleAt(x, y);
	if (water.HasBeenUpdated())
		return;
	water.setHasBeenUpdated(true);

	if (y + 1 < GRID_HEIGHT)  // Check downward
	{	
		// Try to fall as far as velocity allows
		int maxFallDistance = static_cast<int>(water.getVelocity().y);
		if (maxFallDistance <= 0)
			maxFallDistance = 1;

		int fallDistance = 0;
		for (int i = 1; i <= maxFallDistance && y + i < GRID_HEIGHT; ++i)
		{
			Particle& below = getParticleAt(x, y + i);
			if (below.getId() == MAT_ID_EMPTY)
				fallDistance = i;
			else
				break;
		}

		// If we can fall straight down, do it
		if (fallDistance > 0)
		{
			if (fallDistance == 1 && getParticleAt(x, y + 1).getId() == MAT_ID_FIRE)
				getParticleAt(x, y + 1).setId(MAT_ID_EMPTY);
			std::swap(water, getParticleAt(x, y + fallDistance));
			return;
		}

		if (rand() % 2 == 0)
		{
			// Try left first
			if (x > 0)
			{
				Particle& belowLeft = getParticleAt(x - 1, y + 1);
				int id = belowLeft.getId();
				if (id == MAT_ID_FIRE)
				{
					belowLeft.setId(MAT_ID_EMPTY);
					return;
				}
				else if (id == MAT_ID_EMPTY)
				{
					std::swap(water, belowLeft);
					return;
				}
			}
			// Then try right
			if (x + 1 < GRID_WIDTH)
			{
				Particle& belowRight = getParticleAt(x + 1, y + 1);
				int id = belowRight.getId();
				if (id == MAT_ID_FIRE)
				{
					belowRight.setId(MAT_ID_EMPTY);
					return;
				}
				else if (id == MAT_ID_EMPTY)
				{
					std::swap(water, belowRight);
					return;
				}
			}
		}
		else
		{
			if (x + 1 < GRID_WIDTH)
			{
				Particle& belowRight = getParticleAt(x + 1, y + 1);
				int id = belowRight.getId();
				if (id == MAT_ID_FIRE)
				{
					belowRight.setId(MAT_ID_EMPTY);
					return;
				}
				else if (id == MAT_ID_EMPTY)
				{
					std::swap(water, belowRight);
					return;
				}
			}
			if (x > 0)
			{
				Particle& belowLeft = getParticleAt(x - 1, y + 1);
				int id = belowLeft.getId();
				if (id == MAT_ID_FIRE)
				{
					belowLeft.setId(MAT_ID_EMPTY);
					return;
				}
				else if (belowLeft.getId() == MAT_ID_EMPTY)
				{
					std::swap(water, belowLeft);
					return;
				}
			}
		}
	}
	
	// Only spread horizontally if we couldn't move down
	int dispersityRate = water.getDispersityRate();
	if (x - dispersityRate > 0 && x + dispersityRate + 1 < GRID_WIDTH)
	{
		if (rand() % 2 == 0)
		{
			for (int i = 1; i <= dispersityRate; ++i)
			{
				Particle& left = getParticleAt(x - i, y);
				int id = left.getId();
				if (id == MAT_ID_FIRE)
				{
					left.setId(MAT_ID_EMPTY);
					return;
				}
				if (id == MAT_ID_EMPTY)
				{
					std::swap(water, left);
					return;
				}
				if (id == MAT_ID_SAND)
					return;
			}
		}
		else
		{
			for (int i = 1; i <= dispersityRate; ++i)
			{
				Particle& right = getParticleAt(x + i, y);
				int id = right.getId();
				if (id == MAT_ID_FIRE)
				{
					right.setId(MAT_ID_EMPTY);
					return;
				}
				if (id == MAT_ID_EMPTY)
				{
					std::swap(water, right);
					return;
				}
				if (id == MAT_ID_SAND)
					return;
			}
		}
	}
	
	// Move left at the end if no other movement occurred (only when timer allows)
	if (shouldMoveLeftThisFrame)
	{
		if (x > 0)
		{
			Particle& left = getParticleAt(x - 1, y);
			if (left.getId() == MAT_ID_EMPTY)
			{
				std::swap(water, left);
				return;
			}
		}
		else if (x == 0)
		{
			// Delete particle at left edge
			water.setId(MAT_ID_EMPTY);
		}
	}
}

void ParticleWorld::updateWood(float dt, int x, int y)
{
	Particle& wood = getParticleAt(x, y);
	if (wood.HasBeenUpdated())
		return;
	wood.setHasBeenUpdated(true);

	if (wood.getIsOnFire())
	{
		// Check if wood has burned completely
		if (wood.burn(dt))
		{
			wood.setId(MAT_ID_FIRE);
			wood.setLifetime(MAT_FIRE_LIFETIME);
			wood.setIsOnFire(false);
			return;
		}
		
		// Fire spreading - just set the fire flag, don't change material type
		if (rand() % 8 == 0 && y + 1 >= 0 && y + 1 < GRID_HEIGHT)
		{
			Particle& below = getParticleAt(x, y + 1);
			if (below.getId() == MAT_ID_WOOD && !below.getIsOnFire())
			{
				below.setIsOnFire(true);
			}
		}
		if (rand() % 8 == 0 && x - 1 >= 0 && x - 1 < GRID_WIDTH)
		{
			Particle& left = getParticleAt(x - 1, y);
			if (left.getId() == MAT_ID_WOOD && !left.getIsOnFire())
			{
				left.setIsOnFire(true);
			}
		}
		if (rand() % 8 == 0 && x + 1 >= 0 && x + 1 < GRID_WIDTH)
		{
			Particle& right = getParticleAt(x + 1, y);
			if (right.getId() == MAT_ID_WOOD && !right.getIsOnFire())
			{
				right.setIsOnFire(true);
			}
		}
		if (rand() % 8 == 0 && y - 1 >= 0 && y - 1 < GRID_HEIGHT)
		{
			Particle& above = getParticleAt(x, y - 1);
			if (above.getId() == MAT_ID_WOOD && !above.getIsOnFire())
			{
				above.setIsOnFire(true);
			}
		}
		
		return;
	}
}

void ParticleWorld::updateFire(float dt, int x, int y)
{
	Particle& fire = getParticleAt(x, y);
	if (fire.HasBeenUpdated())
		return;
	fire.setHasBeenUpdated(true);
	int hasSpread = false;

	if (fire.burn(dt))
	{
		fire.setId(MAT_ID_EMPTY);
		return;
	}

	if (y + 1 < GRID_HEIGHT)
	{
		Particle& below = getParticleAt(x, y + 1);
		int id = below.getId();
		if (id == MAT_ID_WATER)
		{
			fire.setId(MAT_ID_EMPTY);
			return;	
		}
		if (id == MAT_ID_WOOD || id == MAT_ID_OIL)
		{
			below.setIsOnFire(true);
			hasSpread = true;
		}
	}
	if (x - 1 >= 0)
	{
		Particle& left = getParticleAt(x - 1, y);
		int id = left.getId();
		if (id == MAT_ID_WATER)
		{
			fire.setId(MAT_ID_EMPTY);
			return;	
		}
		if (id == MAT_ID_WOOD || id == MAT_ID_OIL)
		{
			left.setIsOnFire(true);
			hasSpread = true;
		}
	}
	if (x + 1 < GRID_WIDTH)
	{
		Particle& right = getParticleAt(x + 1, y);
		int id = right.getId();
		if (id == MAT_ID_WATER)
		{
			fire.setId(MAT_ID_EMPTY);
			return;	
		}
		if (id == MAT_ID_WOOD || id == MAT_ID_OIL)
		{
			right.setIsOnFire(true);
			hasSpread = true;
		}
	}
	if (y - 1 >= 0)
	{
		Particle& above = getParticleAt(x, y - 1);
		int id = above.getId();
		if (id == MAT_ID_WATER)
		{
			fire.setId(MAT_ID_EMPTY);
			return;	
		}
		if (id == MAT_ID_WOOD || id == MAT_ID_OIL)
		{
			// Just set the fire flag, don't change material type
			above.setIsOnFire(true);
			hasSpread = true;
		}
	}
	if (hasSpread)
	{
		fire.setId(MAT_ID_EMPTY);
		return;
	}
	
	// Try to fall down
	if (y + 1 < GRID_HEIGHT)
	{
		Particle& below = getParticleAt(x, y + 1);
		if (below.getId() == MAT_ID_EMPTY)
		{
			std::swap(particles[x][y], particles[x][y + 1]);
			return;	
		}
	}
	
	// Try diagonal fall
	if (y + 1 < GRID_HEIGHT && x - 1 >= 0)
	{
		Particle& belowLeft = getParticleAt(x - 1, y + 1);
		if (belowLeft.getId() == MAT_ID_EMPTY)
		{
			std::swap(particles[x][y], particles[x - 1][y + 1]);
			return;
		}
	}
	if (y + 1 < GRID_HEIGHT && x + 1 < GRID_WIDTH)
	{
		Particle& belowRight = getParticleAt(x + 1, y + 1);
		if (belowRight.getId() == MAT_ID_EMPTY)
		{
			std::swap(particles[x][y], particles[x + 1][y + 1]);
			return;
		}
	}
	
	// Move left at the end if no other movement occurred (only when timer allows)
	if (shouldMoveLeftThisFrame)
	{
		if (x > 0)
		{
			Particle& left = getParticleAt(x - 1, y);
			if (left.getId() == MAT_ID_EMPTY)
			{
				std::swap(particles[x][y], particles[x - 1][y]);
				return;
			}
		}
		else if (x == 0)
		{
			fire.setId(MAT_ID_EMPTY);
		}
	}
}


void ParticleWorld::update(float dt)
{
	// Update leftward movement timer
	leftwardMoveTimer += dt;
	shouldMoveLeftThisFrame = false;
	if (leftwardMoveTimer >= leftwardMoveInterval)
	{
		leftwardMoveTimer = 0.0f;
		shouldMoveLeftThisFrame = true;
	}
	
	for (int y = 0; y < GRID_HEIGHT; ++y)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			Particle& p = getParticleAt(x, y);
			p.setHasBeenUpdated(false);
		}
	}

	frame_count++;
	for (int y = GRID_HEIGHT - 1; y > 0; --y)
	{
		if (frame_count % 2 == 0)
		{
			for (int x = 0; x < GRID_WIDTH; ++x)
			{
				int mat_id = getParticleAt(x, y).getId();
				switch (mat_id)
				{
					case MAT_ID_EMPTY:
						break;
					case MAT_ID_SAND:
						updateSand(x, y);
						break;
					case MAT_ID_WATER:
						updateWater(x, y);
						break;
					case MAT_ID_WOOD:
						updateWood(dt, x, y);
						break;
					case MAT_ID_FIRE:
						updateFire(dt, x, y);
						break;
					case MAT_ID_WOODFIRE:
						updateWood(dt, x, y);
						break;
					default:
						break;}
			}
		}
		else
		{
			for (int x = GRID_WIDTH - 1; x >= 0; --x)
			{
				int mat_id = getParticleAt(x, y).getId();
				switch (mat_id)
				{
					case MAT_ID_EMPTY:
						break;
					case MAT_ID_SAND:
						updateSand(x, y);
						break;
					case MAT_ID_WATER:
						updateWater(x, y);
						break;
					case MAT_ID_WOOD:
						updateWood(dt, x, y);
						break;
					case MAT_ID_FIRE:
						updateFire(dt, x, y);
						break;
					case MAT_ID_WOODFIRE:
						updateWood(dt, x, y);
						break;
					default:
						break;
				}
			}
		}
	}
}

void ParticleWorld::render(sf::RenderTarget &target)
{
	for (int y = GRID_HEIGHT - 1; y > 0; --y)
	{
		for (int x = 0; x < GRID_WIDTH; ++x)
		{
			int mat_id = getParticleAt(x, y).getId();
			switch (mat_id)
			{
				case MAT_ID_EMPTY:
					break;
				case MAT_ID_SAND:
				{
					sf::RectangleShape rectangle(sf::Vector2f(1.f * ParticleScale, 1.f * ParticleScale));
					rectangle.setPosition({static_cast<float>(x) * ParticleScale, static_cast<float>(y) * ParticleScale});
					rectangle.setFillColor(sf::Color(194, 178, 128)); // Beige/tan sand color
					target.draw(rectangle);
					break;
				}
				case MAT_ID_WATER:
				{
					sf::RectangleShape rectangle(sf::Vector2f(1.f * ParticleScale, 1.f * ParticleScale));
					rectangle.setPosition({static_cast<float>(x) * ParticleScale, static_cast<float>(y) * ParticleScale});
					rectangle.setFillColor(sf::Color(0, 105, 148)); // Ocean blue
					target.draw(rectangle);
					break;
				}
				case MAT_ID_WOOD:
				{
					sf::RectangleShape rectangle(sf::Vector2f(1.f * ParticleScale, 1.f * ParticleScale));
					rectangle.setPosition({static_cast<float>(x) * ParticleScale, static_cast<float>(y) * ParticleScale});
					
					// Check if this wood particle is on fire
					Particle& p = getParticleAt(x, y);
					if (p.getIsOnFire())
					{
						// Render as burning wood with fire colors
						if (rand() % 2 == 0)
							rectangle.setFillColor(sf::Color::Yellow);
						else
							rectangle.setFillColor(sf::Color::Red);
					}
					else
					{
						rectangle.setFillColor(sf::Color(70, 50, 30)); // Dark Brown color
					}
					
					target.draw(rectangle);
					break;
				}
				case MAT_ID_FIRE:
				{
					sf::RectangleShape rectangle(sf::Vector2f(1.f * ParticleScale, 1.f * ParticleScale));
					rectangle.setPosition({static_cast<float>(x) * ParticleScale, static_cast<float>(y) * ParticleScale});
					if (rand() % 2 == 0)
						rectangle.setFillColor(sf::Color::Yellow); // Yellow color
					else
						rectangle.setFillColor(sf::Color::Red); // Red color
					target.draw(rectangle);
					break;
				}
				case MAT_ID_WOODFIRE:
				{
					sf::RectangleShape rectangle(sf::Vector2f(1.f * ParticleScale, 1.f * ParticleScale));
					rectangle.setPosition({static_cast<float>(x) * ParticleScale, static_cast<float>(y) * ParticleScale});
					if (rand() % 2 == 0)
						rectangle.setFillColor(sf::Color::Yellow); // Yellow color
					else
						rectangle.setFillColor(sf::Color::Red); // Red color
					target.draw(rectangle);
					break;
				}
				default:
					break;
			}
		}
	}
}
