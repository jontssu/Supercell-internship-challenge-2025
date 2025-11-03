#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

const float MAT_WOOD_LIFETIME = 0.3f;
const float MAT_FIRE_LIFETIME = 0.1f;
const float MAT_SMOKE_LIFETIME = 15.f;


enum MaterialID
{
	MAT_ID_EMPTY = 0,
	MAT_ID_SAND = 1,
	MAT_ID_WATER = 2,
	MAT_ID_WOOD = 3,
	MAT_ID_STONE = 4,
	MAT_ID_OIL = 5,
	MAT_ID_FIRE = 6,
	MAT_ID_WOODFIRE = 7,
	MAT_ID_SMOKE = 8
};

class Particle
{
	public:
	    Particle() = default;
		Particle(int id, float lifetime, sf::Vector2f velocity, sf::Color color);
	    ~Particle() {}

		inline void setHasBeenUpdated(bool updated) { has_been_updated = updated; }
		inline void setVelocity(const sf::Vector2f& vel) { velocity = vel; }
		inline void setIsSideMoving(int val) { isSideMoving = val; }
		inline void setIsOnFire(bool val) { isOnFire = val; }
		inline void setId(int new_id) { id = new_id; }
		inline void setLifetime(float life) { lifetime = life; }

		inline const int getId() const { return id; }
		inline const bool HasBeenUpdated() const { return has_been_updated; }
		inline const sf::Vector2f& getVelocity() const { return velocity; }
		inline const int getIsSideMoving() const { return isSideMoving; }
		inline const int getDispersityRate() const { return dispersityRate; }
		inline const bool getIsFlammable() const { return isFlammable; }
		inline const bool getIsOnFire() const { return isOnFire; }

		bool burn(float dt);
		void update();

	private:
		int				id = 0;
		float			lifetime = 5.f;
		sf::Vector2f	velocity;
		sf::Color		color = sf::Color::White;
		bool			has_been_updated = false;
		int				isSideMoving = 0;
		int			    dispersityRate;
		bool			isFlammable = false;
		bool			isOnFire = false;
};