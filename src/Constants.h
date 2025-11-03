#pragma once

const unsigned int WindowWidth = 800;
const unsigned int WindowHeight = 600;
const unsigned int ParticleScale = 2.0f;

constexpr int GRID_WIDTH = WindowWidth / ParticleScale;
constexpr int GRID_HEIGHT = WindowHeight / ParticleScale;

const float GroundLevel = 500.0f;
const float AttackSpeed = 0.01f;
const float PlayerDamage = 25.0f;
const float ProjectileWidth = 2.0f;
const float ProjectileHeight = 2.0f;
const float ProjectileSpeed = 600.0f; // Pixels per second

const float EnemySpawnInterval = 2.0f;
const float EnemySpeed = 100.0f;
const int EnemyDamage = 10;
const int EnemyHealth = 100;
const int EnemySpawnCount = 5;