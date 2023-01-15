#include "ColorFunctions.hpp"

uint32_t ColorFunctions::rand2() {
	g_lehmer64_state *= 0xe4dd58b5;
	return g_lehmer64_state >> 32;
}

void ColorFunctions::setRandSeed(uint64_t seed)
{
	g_lehmer64_state = seed;
}

glm::u8vec3 ColorFunctions::directCoord(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth)
{
	return { initPos.x % 256, initPos.y % 256, initPos.z % 256 };
}

glm::u8vec3 ColorFunctions::depth(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth)
{
	return { (depth * 20) % 256, (depth * 20) % 256, (depth * 20) % 256 };
}

glm::u8vec3 ColorFunctions::random(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth)
{
	return { rand2() % 256, rand2() % 256, rand2() % 256 };
}

glm::u8vec3 ColorFunctions::randomBlend(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth)
{
	return {
		glm::min(parentColor.x + (rand2() % 40 - 20), 255U),
		glm::min(parentColor.y + (rand2() % 40 - 20), 255U),
		glm::min(parentColor.z + (rand2() % 40 - 20), 255U)
	};
}

uint8_t ColorFunctions::lagrange(long long length, long long point)
{
	long long part1 = (point * 2 - length);
	long long part2 = part1 * part1;
	long long part3 = length * length;
	double part4 = 254.0 / part3;
	long long part5 = part2 * part4;
	return part5 % 256;
}

glm::u8vec3 ColorFunctions::ambientOcclusionEffect(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth)
{
	uint32_t length = (endPos.x - initPos.x) << depth;
	uint8_t value = glm::max(glm::max(
		lagrange(length, initPos.x),
		lagrange(length, initPos.y)),
		lagrange(length, initPos.z));
	return { value, value, value };
}

glm::u8vec3 ColorFunctions::AmbOcclusionColorful(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth)
{
	uint32_t length = (endPos.x - initPos.x) << depth;
	return { lagrange(length, initPos.x), lagrange(length, initPos.y), lagrange(length, initPos.z) };
}

glm::u8vec3 ColorFunctions::white(glm::u8vec3 parentColor, glm::uvec3 initPos, glm::uvec3 endPos, uint8_t depth)
{
	return {255, 255, 255};
}
