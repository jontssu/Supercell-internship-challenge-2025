#include "Player.h"
#include "ResourceManager.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <cmath>
#include <iostream>
#include "Constants.h"
#include "Projectile.h"

Player::Player()
{
}

void Player::initPhysics()
{
	velocityMax = 100.f;
	velocityMin = 1.f;
	acceleration = 100.f;
	drag = 0.80f;
	gravity = 25.f; // Increased gravity for better jump feel
	velocityMaxY = 900.f; // Increased max Y velocity to allow much higher jumps
}

sf::Vector2f Player::getShootDirection() const
{
    sf::Vector2f direction = m_mousePosition - m_position;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    // Normalize the direction
    if (length > 0.0f)
        direction /= length;
    else
        direction = sf::Vector2f(1.0f, 0.0f); // Default to right if mouse is on player
    
    return direction;
}

void Player::shoot(float dt, int type)
{
    if (m_shootCooldown > 0.0f)
        m_shootCooldown -= dt;
        
    if (m_shootCooldown <= 0.0f)
    {
        sf::Vector2f direction = getShootDirection();
        float projectileSpeed = ProjectileSpeed; // Pixels per second
        
        m_projectileRequest.position = m_position;
        m_projectileRequest.velocity = direction * projectileSpeed;
        m_projectileRequest.projectileType = type;  // Set the type!
        m_hasProjectileRequest = true;
        
        m_shootCooldown = m_attackSpeed;
    }
}

bool Player::init()
{
    const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("player.png");
    if (pTexture == nullptr)
        return false;

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
        return false;

    m_rotation = sf::degrees(0);
    sf::FloatRect localBounds = m_pSprite->getLocalBounds();
    m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
    m_pSprite->setPosition(m_position);
    m_pSprite->setScale(sf::Vector2f(3.0f, 3.0f));
    m_collisionRadius = collisionRadius;

    // Initialize physics
    initPhysics();

    return true;
}

void Player::updatePhysics(float dt)
{
	//Gravity
	velocity.y += 1.0 * gravity;
	if (std::abs(velocity.y) > velocityMaxY)
		velocity.y = (velocity.y > 0) ? velocityMaxY : -velocityMaxY;

	//Deceleration
	velocity *= drag;

	//Limit deceleration
	if (std::abs(velocity.x) < velocityMin)
		velocity.x = 0.f;
	if (std::abs(velocity.y) < velocityMin)
		velocity.y = 0.f;

	if (std::abs(velocity.x) <= 1.f)
		velocity.x = 0.f;
	// sprite.move(velocity);
}

void Player::update(float dt)
{
    // Check for particle collisions if particle world is available
    m_inWater = false;
    m_groundLevel = GroundLevel;
    bool standingOnGround = false;
    bool touchingSandOrWater = false;
    
    if (m_pParticleWorldPtr)
    {
        // Check particles below and around the player
        int playerGridX = static_cast<int>(m_position.x / ParticleScale);
        int playerGridY = static_cast<int>(m_position.y / ParticleScale);
        
        // Check for sand/water below player to stand on
        float highestSandY = GroundLevel;
        bool standingOnSand = false;
        
        // Check a wider area below the player
        int checkRadius = static_cast<int>(collisionRadius / ParticleScale);
        for (int dx = -checkRadius; dx <= checkRadius; ++dx)
        {
            for (int dy = 0; dy <= checkRadius * 2; ++dy)
            {
                int gridX = playerGridX + dx;
                int gridY = playerGridY + dy;
                
                if (gridX >= 0 && gridX < GRID_WIDTH && gridY >= 0 && gridY < GRID_HEIGHT)
                {
                    int matId = m_pParticleWorldPtr->getParticleAt(gridX, gridY).getId();
                    
                    // Check if player is in water
                    if (matId == MAT_ID_WATER)
                    {
                        float particleWorldY = gridY * ParticleScale;
                        if (particleWorldY >= m_position.y - collisionRadius && 
                            particleWorldY <= m_position.y + collisionRadius)
                        {
                            m_inWater = true;
                            touchingSandOrWater = true;
                        }
                    }
                    
                    // Check for sand to stand on
                    if (matId == MAT_ID_SAND || matId == MAT_ID_WATER)
                    {
                        float particleWorldY = gridY * ParticleScale;
                        float particleWorldX = gridX * ParticleScale;
                        
                        // Check if touching sand/water particles
                        float dx_real = particleWorldX - m_position.x;
                        float dy_real = particleWorldY - m_position.y;
                        float distSq = dx_real * dx_real + dy_real * dy_real;
                        
                        if (distSq < collisionRadius * collisionRadius)
                        {
                            touchingSandOrWater = true;
                        }
                        
                        // Check if particle is directly below player
                        if (particleWorldY > m_position.y && 
                            std::abs(particleWorldX - m_position.x) < collisionRadius)
                        {
                            if (particleWorldY < highestSandY)
                            {
                                highestSandY = particleWorldY;
                                standingOnSand = true;
                            }
                        }
                    }
                }
            }
        }
        
        if (standingOnSand)
        {
            m_groundLevel = highestSandY - 5.0f; 
        }
    }
    
    // Check if player is on the ground
    standingOnGround = (m_position.y >= m_groundLevel - 1.0f);
    
    // Handle input for horizontal movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        if (velocity.x > -velocityMax)
            velocity.x -= acceleration * dt * 60.0f;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        if (velocity.x < velocityMax)
            velocity.x += acceleration * dt * 60.0f; 
    }
    
    if (touchingSandOrWater)
    {
        float basePushForce = BasePushForce;
        float pushIncreasePerTenSeconds = 0.2f;
        float pushForce = basePushForce + (m_gameTime / 10.0f) * pushIncreasePerTenSeconds;
        velocity.x -= pushForce;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
    {
        if (standingOnGround)
        {
            // Jump - apply upward velocity
            float jumpForce = -600.0f; 
            if (m_inWater)
                jumpForce *= 0.6f; // Weaker jump in water
            velocity.y = jumpForce;
        }
    }
    
    // Apply water effects to physics
    float currentDrag = drag;
    float currentGravity = gravity;
    
    if (m_inWater)
    {
        currentDrag = 0.92f;
        currentGravity = gravity * 0.4f; 
        
        // Clamp vertical velocity in water
        if (velocity.y > velocityMaxY * 0.5f)
            velocity.y = velocityMaxY * 0.5f;
    }
    
    // Apply physics
    velocity.y += 1.0f * currentGravity * dt * 60.0f; // Scale by 60 for frame-rate independent
    if (std::abs(velocity.y) > velocityMaxY)
        velocity.y = (velocity.y > 0) ? velocityMaxY : -velocityMaxY;
    
    // Apply drag 
    float dragFactor = std::pow(currentDrag, dt * 60.0f);
    velocity *= dragFactor;
    
    // Limit deceleration
    if (std::abs(velocity.x) < velocityMin)
        velocity.x = 0.f;
    if (std::abs(velocity.y) < velocityMin)
        velocity.y = 0.f;
    
    // Apply velocity to position
    m_position += velocity * dt;
    
    // Ground collision
    if (m_position.y > m_groundLevel)
    {
        m_position.y = m_groundLevel;
        velocity.y = 0.f;
    }
    
    // Screen bounds
    if (m_position.x < 50.0f)
        m_position.x = 50.0f;
    if (m_position.x > WindowWidth - 50.0f)
        m_position.x = WindowWidth - 50.0f;

    // Shooting
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        shoot(dt, PROJECTILE_TYPE_WATER);
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        shoot(dt, PROJECTILE_TYPE_FIRE);
}

void Player::render(sf::RenderTarget& target) const
{
    if (const sf::RenderWindow* window = dynamic_cast<const sf::RenderWindow*>(&target))
        m_mousePosition = window->mapPixelToCoords(sf::Mouse::getPosition(*window));
    
    m_pSprite->setRotation(m_rotation);
    m_pSprite->setPosition(m_position);
    target.draw(*m_pSprite);
}
