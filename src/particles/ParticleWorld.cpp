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
	
	// Verify it was set correctly
	int storedId = particles[x][y].getId();
	std::cout << "Added particle with mat_id=" << mat_id << ", verified stored id=" << storedId << " at grid[" << x << "][" << y << "]" << std::endl;
}

void ParticleWorld::updateSand(int x, int y)
{
	Particle& sand = getParticleAt(x, y);
	if (sand.HasBeenUpdated())
		return;
	sand.setHasBeenUpdated(true);

	if (y + 1 < GRID_HEIGHT)
	{
		// Try to fall as far as velocity allows (default to 5 if velocity is 0)
		int maxFallDistance = static_cast<int>(sand.getVelocity().y);
		if (maxFallDistance <= 0)
			maxFallDistance = 1;

		int fallDistance = 0;
		for (int i = 1; i <= maxFallDistance && y + i < GRID_HEIGHT; ++i)
		{
			Particle& below = getParticleAt(x, y + i);
			// Only fall through empty spaces, STOP at water or sand
			if (below.getId() == MAT_ID_EMPTY)
				fallDistance = i;
			else
				break;  // Hit an obstacle
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

		// Can't fall straight, try diagonal
		Particle& belowLeft = getParticleAt(x - 1, y + 1);
		if (belowLeft.getId() == MAT_ID_EMPTY
			|| belowLeft.getId() == MAT_ID_WATER)
		{
			std::swap(sand, belowLeft);
			return;
		}

		Particle& belowRight = getParticleAt(x + 1, y + 1);
		if (belowRight.getId() == MAT_ID_EMPTY
			|| belowRight.getId() == MAT_ID_WATER)
		{
			std::swap(sand, belowRight);
			return;
		}
	}
}

void ParticleWorld::updateWater(int x, int y)
{
	Particle& water = getParticleAt(x, y);
	if (water.HasBeenUpdated())
		return;
	water.setHasBeenUpdated(true);

	if (y + 1 < GRID_HEIGHT)
	{	
		// Try to fall as far as velocity allows (default to 5 if velocity is 0)
		int maxFallDistance = static_cast<int>(water.getVelocity().y);
		if (maxFallDistance <= 0)
			maxFallDistance = 1;

		int fallDistance = 0;
		for (int i = 1; i <= maxFallDistance && y + i < GRID_HEIGHT; ++i)
		{
			Particle& below = getParticleAt(x, y + i);
			// Only fall through empty spaces, STOP at water or sand
			if (below.getId() == MAT_ID_EMPTY)
				fallDistance = i;
			else
				break;  // Hit an obstacle
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
}

void ParticleWorld::updateWood(float dt, int x, int y)
{
	Particle& wood = getParticleAt(x, y);
	if (wood.HasBeenUpdated())
		return;
	wood.setHasBeenUpdated(true);


	if (wood.getIsOnFire())
	{
		if (y - 1 >= 0)
		{
			Particle& above = getParticleAt(x, y - 1);
			if (above.getId() == MAT_ID_EMPTY && rand() % 5 == 0)
				above.setId(MAT_ID_SMOKE);
		}
		if (wood.burn(dt))
		{
			wood.setId(MAT_ID_FIRE);
			wood.setLifetime(MAT_FIRE_LIFETIME);
		}
		if (y + 1 < GRID_HEIGHT && rand() % 4 == 0)
		{
			Particle& below = getParticleAt(x, y + 1);
			if (below.getId() == MAT_ID_WOOD)
			{
				below.setId(MAT_ID_WOODFIRE);
				below.setIsOnFire(true);
			}
		}
		if (x - 1 >= 0 && rand() % 4 == 0)
		{
			Particle& left = getParticleAt(x - 1, y);
			if (left.getId() == MAT_ID_WOOD)
			{
				left.setId(MAT_ID_WOODFIRE);
				left.setIsOnFire(true);
			}
		}
		if (x + 1 < GRID_WIDTH && rand() % 4 == 0)
		{
			Particle& right = getParticleAt(x + 1, y);
			if (right.getId() == MAT_ID_WOOD)
			{
				right.setId(MAT_ID_WOODFIRE);
				right.setIsOnFire(true);
			}
		}
		if (y - 1 >= 0 && rand() % 4 == 0)
		{
			Particle& above = getParticleAt(x, y - 1);
			if (above.getId() == MAT_ID_WOOD)
			{
				above.setId(MAT_ID_WOODFIRE);
				above.setIsOnFire(true);
			}
		}
	}
	
	// Move wood left
	if (x - 1 >= 0)
	{
		Particle& left = getParticleAt(x - 1, y);
		if (left.getId() == MAT_ID_EMPTY)
		{
			// Swap wood particle with empty space on the left
			std::swap(particles[x][y], particles[x - 1][y]);
			getParticleAt(x - 1, y).setHasBeenUpdated(true);
		}
	}
}

void ParticleWorld::updateStone(int x, int y) 
{
}

void ParticleWorld::updateOil(int x, int y)
{
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

	Particle& below = getParticleAt(x, y + 1);
	if (y + 1 < GRID_HEIGHT)
	{
		int id = below.getId();
		if (id == MAT_ID_WATER)
		{
			fire.setId(MAT_ID_EMPTY);
			return;	
		}
		if (id == MAT_ID_WOOD || id == MAT_ID_OIL)
		{
			if (id == MAT_ID_WOOD)
				below.setId(MAT_ID_WOODFIRE);
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
			if (id == MAT_ID_WOOD)
				left.setId(MAT_ID_WOODFIRE);
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
			if (id == MAT_ID_WOOD)
				right.setId(MAT_ID_WOODFIRE);
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
			if (id == MAT_ID_WOOD)
				above.setId(MAT_ID_WOODFIRE);
			above.setIsOnFire(true);
			hasSpread = true;
		}
	}
	if (hasSpread)
	{
		fire.setId(MAT_ID_EMPTY);
		return;
	}
	if (below.getId() == MAT_ID_EMPTY)
	{
		std::swap(fire, below);
		return;	
	}
	Particle& belowLeft = getParticleAt(x - 1, y + 1);
	if (belowLeft.getId() == MAT_ID_EMPTY)
	{
		std::swap(fire, belowLeft);
		return;
	}
	Particle& belowRight = getParticleAt(x + 1, y + 1);
	if (belowRight.getId() == MAT_ID_EMPTY)
	{
		std::swap(fire, belowRight);
		return;
	}
}

void ParticleWorld::updateSmoke(float dt, int x, int y)
{
	Particle& smoke = getParticleAt(x, y);
	if (smoke.HasBeenUpdated())
		return;
	smoke.setHasBeenUpdated(true);

	if (smoke.burn(dt))
	{
		smoke.setId(MAT_ID_EMPTY);
		smoke.setLifetime(10.f);
	}

	Particle& above = getParticleAt(x, y - 1);
	if (above.getId() == MAT_ID_EMPTY)
	{
		std::swap(smoke, above);
		return;	
	}
	Particle& aboveLeft = getParticleAt(x - 1, y - 1);
	if (aboveLeft.getId() == MAT_ID_EMPTY)
	{
		std::swap(smoke, aboveLeft);
		return;
	}
	Particle& aboveRight = getParticleAt(x + 1, y - 1);
	if (aboveRight.getId() == MAT_ID_EMPTY)
	{
		std::swap(smoke, aboveRight);
		return;
	}
	Particle& left = getParticleAt(x - 1, y);
	if (left.getId() == MAT_ID_EMPTY)
	{
		std::swap(smoke, left);
		return;
	}	
	Particle& right = getParticleAt(x + 1, y);
	if (right.getId() == MAT_ID_EMPTY)
	{
		std::swap(smoke, right);
		return;
	}
}


void ParticleWorld::update(float dt)
{
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
					case MAT_ID_SMOKE:
					{
						if (rand() % 4 == 0)
							break;
						updateSmoke(dt, x, y);
						break;
					}
				}
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
					case MAT_ID_SMOKE:
					{
						if (rand() % 4 == 0)
							break;
						updateSmoke(dt, x, y);
						break;
					}
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
					rectangle.setPosition({static_cast<float>(x), static_cast<float>(y)});
					rectangle.setFillColor(sf::Color(70, 50, 30)); // Dark Brown color
					target.draw(rectangle);
					break;
				}
				case MAT_ID_FIRE:
				{
					sf::RectangleShape rectangle(sf::Vector2f(1.f * ParticleScale, 1.f * ParticleScale));
					rectangle.setPosition({static_cast<float>(x), static_cast<float>(y)});
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
					rectangle.setPosition({static_cast<float>(x), static_cast<float>(y)});
					if (rand() % 2 == 0)
						rectangle.setFillColor(sf::Color::Yellow); // Yellow color
					else
						rectangle.setFillColor(sf::Color::Red); // Red color
					target.draw(rectangle);
					break;
				}
				case MAT_ID_SMOKE:
				{
					sf::RectangleShape rectangle(sf::Vector2f(1.f * ParticleScale, 1.f * ParticleScale));
					rectangle.setPosition({static_cast<float>(x), static_cast<float>(y)});
					rectangle.setFillColor(sf::Color(0, 0, 0)); // Black color
					target.draw(rectangle);
					break;
				}
				default:
					break;
			}
		}
	}
}
