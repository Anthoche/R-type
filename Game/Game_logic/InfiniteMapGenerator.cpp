/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** InfiniteMapGenerator
*/

#include "InfiniteMapGenerator.hpp"
#include <array>
#include <cmath>

namespace {
	struct EnemyTemplate {
		std::string image;
		std::string type;
		std::string pattern;
		int health;
		float speed;
		float width;
		float height;
	};

	struct ObstacleTemplate {
		std::string image;
		float width;
		float height;
		float speed;
	};

	struct PickupTemplate {
		std::string image;
		std::string type;
		float width;
		float height;
	};

	const std::array<EnemyTemplate, 6> ENEMY_LIBRARY{{
		{"../Game/Assets/sprites/ennemies/r-typesheet19.png", "basic", "straight", 3, 0.65f, 33.f, 28.f},
		{"../Game/Assets/sprites/ennemies/r-typesheet14.png", "zigzag", "zigzag", 2, 0.55f, 50.f, 50.f},
		{"../Game/Assets/sprites/ennemies/r-typesheet11.png", "turret", "turret", 4, 0.35f, 33.f, 33.f},
		{"../Game/Assets/sprites/ennemies/r-typesheet7.png", "zigzag", "zigzag", 3, 0.7f, 33.f, 33.f},
		{"../Game/Assets/sprites/ennemies/r-typesheet17.png", "spiral", "spiral", 2, 0.6f, 42.f, 42.f},
		{"../Game/Assets/sprites/ennemies/r-typesheet16.png", "basic", "straight", 5, 0.5f, 60.f, 38.f}
	}};

	const std::array<ObstacleTemplate, 4> OBSTACLE_LIBRARY{{
		{"../Game/Assets/sprites/space_rock.png", 220.f, 70.f, 0.55f},
		{"../Game/Assets/sprites/asteroid_large.png", 180.f, 180.f, 0.4f},
		{"../Game/Assets/sprites/asteroid_medium.png", 120.f, 120.f, 0.45f},
		{"../Game/Assets/sprites/crystal.png", 90.f, 210.f, 0.35f}
	}};

	const std::array<PickupTemplate, 2> PICKUP_LIBRARY{{
		{"../Game/Assets/sprites/heart.png", "health", 18.f, 18.f},
		{"../Game/Assets/sprites/powerup.png", "powerup", 26.f, 26.f}
	}};

	const std::array<const char *, 3> BACKGROUND_IMAGES{{
		"../Game/Assets/sprites/lvl1_Backgrounds.png",
		"../Game/Assets/sprites/lvl2_Background.png",
		"../Game/Assets/sprites/lvl3_Background.png"
	}};
}

namespace game::generation {

	InfiniteMapGenerator::InfiniteMapGenerator(std::uint32_t seed)
		: _baseSeed(seed == 0 ? std::random_device{}() : seed) {}

	void InfiniteMapGenerator::reseed(std::uint32_t seed) {
		_baseSeed = seed == 0 ? std::random_device{}() : seed;
	}

	float InfiniteMapGenerator::randomFloat(std::mt19937 &rng, float min, float max) const {
		std::uniform_real_distribution<float> dist(min, max);
		return dist(rng);
	}

	int InfiniteMapGenerator::randomInt(std::mt19937 &rng, int min, int max) const {
		std::uniform_int_distribution<int> dist(min, max);
		return dist(rng);
	}

	nlohmann::json InfiniteMapGenerator::buildBackgroundLayer(float chunkStartX) const {
		std::size_t index = static_cast<std::size_t>(std::fmod(chunkStartX / _chunkWidth, BACKGROUND_IMAGES.size()));
		nlohmann::json layer = {
			{"x", chunkStartX + _chunkWidth / 2.f},
			{"y", 320.f},
			{"z", 0.f},
			{"width", _chunkWidth * 1.2f},
			{"height", 320.f},
			{"depth", 50.f},
			{"image_path", BACKGROUND_IMAGES[index]},
			{"model_path", ""},
			{"scale", 1.0f},
			{"scroll_speed", 0.35f + 0.05f * static_cast<float>(index)}
		};
		return layer;
	}

	void InfiniteMapGenerator::populateEnemies(nlohmann::json &container, std::mt19937 &rng, float chunkStartX) const {
		int waveCount = randomInt(rng, 3, 5);
		for (int wave = 0; wave < waveCount; ++wave) {
			int enemiesPerWave = randomInt(rng, 2, 5);
			float waveOffset = randomFloat(rng, 300.f, _chunkWidth - 400.f);
			for (int i = 0; i < enemiesPerWave; ++i) {
				const auto &templ = ENEMY_LIBRARY[static_cast<std::size_t>(randomInt(rng, 0,
													 static_cast<int>(ENEMY_LIBRARY.size() - 1)))];
				float x = chunkStartX + waveOffset + i * randomFloat(rng, 80.f, 150.f);
				float y = randomFloat(rng, _playfieldMinY, _playfieldMaxY);

				container.push_back({
					{"x", x},
					{"y", y},
					{"image_path", templ.image},
					{"type", templ.type},
					{"pattern", templ.pattern},
					{"health", templ.health},
					{"speed", templ.speed},
					{"w", templ.width},
					{"h", templ.height}
				});
			}
		}
	}

	void InfiniteMapGenerator::populateObstacles(nlohmann::json &container, std::mt19937 &rng, float chunkStartX) const {
		int obstacleCount = randomInt(rng, 4, 8);
		for (int i = 0; i < obstacleCount; ++i) {
			const auto &templ = OBSTACLE_LIBRARY[static_cast<std::size_t>(randomInt(rng, 0,
												 static_cast<int>(OBSTACLE_LIBRARY.size() - 1)))];
			float x = chunkStartX + randomFloat(rng, 150.f, _chunkWidth - 150.f);
			float y = randomFloat(rng, _playfieldMinY, _playfieldMaxY);

			container.push_back({
				{"x", x},
				{"y", y},
				{"image_path", templ.image},
				{"speed", templ.speed},
				{"w", templ.width},
				{"h", templ.height}
			});
		}
	}

	void InfiniteMapGenerator::populatePickups(nlohmann::json &container, std::mt19937 &rng, float chunkStartX) const {
		int pickupCount = randomInt(rng, 1, 3);
		for (int i = 0; i < pickupCount; ++i) {
			const auto &templ = PICKUP_LIBRARY[static_cast<std::size_t>(randomInt(rng, 0,
												 static_cast<int>(PICKUP_LIBRARY.size() - 1)))];
			float x = chunkStartX + randomFloat(rng, 200.f, _chunkWidth - 200.f);
			float y = randomFloat(rng, _playfieldMinY, _playfieldMaxY);

			container.push_back({
				{"x", x},
				{"y", y},
				{"image_path", templ.image},
				{"type", templ.type},
				{"speed", 0.4f},
				{"w", templ.width},
				{"h", templ.height}
			});
		}
	}

	nlohmann::json InfiniteMapGenerator::generateChunk(std::size_t chunkIndex) const {
		std::mt19937 rng(_baseSeed + static_cast<std::uint32_t>(chunkIndex * 7919u + 97u));
		float chunkStartX = static_cast<float>(chunkIndex) * _chunkWidth;

		nlohmann::json chunk;
		auto &level = chunk["level"];
		level["backgrounds"] = nlohmann::json::array();
		level["enemies"] = nlohmann::json::array();
		level["obstacles"] = nlohmann::json::array();
		level["random_elements"] = nlohmann::json::array();
		level["sounds"] = nlohmann::json::array();

		level["sounds"].push_back({
			{"sound_path", "../Game/Assets/sounds/BATTLE PRESSURE.mp3"},
			{"volume", 0.35f},
			{"loop", true},
			{"autoplay", chunkIndex == 0}
		});

		level["backgrounds"].push_back(buildBackgroundLayer(chunkStartX));
		populateEnemies(level["enemies"], rng, chunkStartX);
		populateObstacles(level["obstacles"], rng, chunkStartX);
		populatePickups(level["random_elements"], rng, chunkStartX);

		return chunk;
	}
}
