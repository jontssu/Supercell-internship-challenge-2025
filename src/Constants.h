#pragma once

const unsigned int WindowWidth = 800;
const unsigned int WindowHeight = 600;
const unsigned int ParticleScale = 4.0f;

constexpr int GRID_WIDTH = WindowWidth / ParticleScale;
constexpr int GRID_HEIGHT = WindowHeight / ParticleScale;

const float GroundLevel = 576.0f;
const float AttackSpeed = 0.01f;
const float PlayerDamage = 25.0f;
const float ProjectileWidth = 1.0f;
const float ProjectileHeight = 1.0f;
const float ProjectileSpeed = 600.0f; // Pixels per second

const float EnemySpawnInterval = 3.0f;
const float EnemySpeed = 150.0f;
const int EnemyDamage = 10;
const int EnemyHealth = 25;
const int EnemySpawnCount = 2;

const float BasePushForce = 7.5f;