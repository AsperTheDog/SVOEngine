#pragma once

#include <glm/glm.hpp>

class ColorFunctions
{
	static uint8_t lagrange(long long length, long long point);

public:
	inline static uint64_t g_lehmer64_state = 1;
	
	static void setRandSeed(uint64_t seed);
	static uint32_t rand2();

	static glm::u8vec3 directCoord(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth);
	static glm::u8vec3 depth(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth);
	static glm::u8vec3 random(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth);
	static glm::u8vec3 randomBlend(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth);
	static glm::u8vec3 ambientOcclusionEffect(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth);
	static glm::u8vec3 AmbOcclusionColorful(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth);
	static glm::u8vec3 white(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth);
	
};
