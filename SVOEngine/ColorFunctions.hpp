#pragma once

#include <glm/glm.hpp>

inline uint64_t g_lehmer64_state = 1;


inline uint32_t rand2() {
  g_lehmer64_state *= 0xe4dd58b5;
  return g_lehmer64_state >> 32;
}

inline void setRandSeed(uint64_t seed)
{
	g_lehmer64_state = seed;
}

inline glm::u8vec3 color_directCoord(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth)
{
	return {initPos.x % 256, initPos.y % 256, initPos.z % 256};
}

inline glm::u8vec3 color_depth(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth)
{
	return {(depth * 20) % 256, (depth * 20) % 256, (depth * 20) % 256};
}

inline glm::u8vec3 color_random(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth)
{
	return {rand2() % 256, rand2() % 256, rand2() % 256};
}

inline glm::u8vec3 color_randomBlend(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth)
{
	return {
		glm::min(parentColor.x + (rand2() % 40 - 20), 255U),
		glm::min(parentColor.y + (rand2() % 40 - 20), 255U),
		glm::min(parentColor.z + (rand2() % 40 - 20), 255U)
	};
}

inline glm::u8vec3 color_randomBrightnessBlend(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth)
{
	int randInt = ((rand2() % 40) - 20);
	float random = (float)randInt / 40.f + 1;
	return {
		glm::min((uint32_t)(parentColor.x * random), 255U),
		glm::min((uint32_t)(parentColor.y * random), 255U),
		glm::min((uint32_t)(parentColor.z * random), 255U)
	};
}