#pragma once

const unsigned int WindowWidth = 800;
const unsigned int WindowHeight = 600;
const unsigned int ParticleScale = 2.0f;

constexpr int GRID_WIDTH = WindowWidth / ParticleScale;
constexpr int GRID_HEIGHT = WindowHeight / ParticleScale;

const float GroundLevel = 500.0f;
const float AttackSpeed = 0.2f;
const float ProjectileWidth = 2.0f;
const float ProjectileHeight = 2.0f;