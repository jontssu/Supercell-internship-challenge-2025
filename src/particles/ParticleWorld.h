#pragma once

#include "Particle.h"
#include <vector>
#include <SFML/System/Vector2.hpp>

namespace sf { class RenderTarget; }

class ParticleWorld 
{
	public:
	    ParticleWorld();
	    ~ParticleWorld() {};

		Particle &getParticleAt(int x, int y);
		ParticleWorld& getParticleWorld() { return *this; }

		void addParticle(const sf::Vector2f& position, sf::Vector2f velocity, int mat_id);

		void updateSand(int x, int y);
		void updateWater(int x, int y);
		void updateWood(float dt, int x, int y);
		void updateFire(float dt, int x, int y);
		void updateSmoke(float dt, int x, int y);

	    void update(float deltaTime);
	    void render(sf::RenderTarget &target);

	  private:
		std::vector<std::vector<Particle>>	particles;
		int									frame_count = 0;
		sf::Vector2f						gravity = {0.f, 1.f};  // Positive = downward
		float								leftwardMoveTimer = 0.0f;
		float								leftwardMoveInterval = 0.02f; // Move left every 0.02 seconds
		bool								shouldMoveLeftThisFrame = false;
};