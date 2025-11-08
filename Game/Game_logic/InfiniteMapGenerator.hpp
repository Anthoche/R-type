/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** InfiniteMapGenerator
*/

#ifndef RTYPE_INFINITEMAPGENERATOR_HPP
#define RTYPE_INFINITEMAPGENERATOR_HPP

#include <nlohmann/json.hpp>
#include <random>
#include <string>
#include <vector>

namespace game::generation {

class InfiniteMapGenerator {
	public:
		explicit InfiniteMapGenerator(std::uint32_t seed = std::random_device{}());

		void reseed(std::uint32_t seed);

		[[nodiscard]] nlohmann::json generateChunk(std::size_t chunkIndex) const;

	private:
		std::uint32_t _baseSeed;
		float const _chunkWidth = 2400.f;
		float const _playfieldMinY = 100.f;
		float const _playfieldMaxY = 900.f;

		float randomFloat(std::mt19937 &rng, float min, float max) const;
		int randomInt(std::mt19937 &rng, int min, int max) const;

		nlohmann::json buildBackgroundLayer(float chunkStartX) const;
		void populateEnemies(nlohmann::json &container, std::mt19937 &rng, float chunkStartX) const;
		void populateObstacles(nlohmann::json &container, std::mt19937 &rng, float chunkStartX) const;
		void populatePickups(nlohmann::json &container, std::mt19937 &rng, float chunkStartX) const;
};

} // namespace game::generation

#endif // RTYPE_INFINITEMAPGENERATOR_HPP
