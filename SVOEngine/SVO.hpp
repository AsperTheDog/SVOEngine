#pragma once
#include <cstdint>

#include <glm/glm.hpp>

#include "SVOAddons.hpp"

class SVO;
class SVONode;

struct Ray
{
	glm::vec3 origin;
	glm::vec3 direction;
	glm::vec3 directionInv;
	glm::u8vec3 color;
	float t;
};

class SVONode
{
public:
	SVONode() : childrenLocator({0}) {}

	void setChildren(ChildIndexer indexes, uint32_t childPos);
	void setColor(glm::u8vec3 color);
	bool isChildAt(uint8_t pos);
	uint32_t getChildrenIndex();
	bool intersectRay(Ray* ray, glm::uvec3 initPos, glm::uvec3 endPos, SVO* parent);

private:
	uint32_t childIndex;
	glm::u8vec3 color{};
	ChildIndexer childrenLocator;
};

class SVO
{
public:
	explicit SVO(uint8_t maxDepth, uint64_t maxMemBytes, glm::u8vec3 (*getColor)(glm::u8vec3, glm::uvec3, glm::uvec3, uint8_t depth));

	void populate(uint8_t sizeMult);
	void seal();
	SVONode* getRoot();
	SVONode* getChild(uint32_t index, uint8_t offset);
	bool intersectRay(Ray* ray, uint8_t sizeMult);

	int LODLevel;

private:
	SVONode root{};
	NodeIndexer indexer;
	uint8_t maxDepth;
	glm::u8vec3 (*getColor)(glm::u8vec3, glm::uvec3, glm::uvec3, uint8_t depth);

	void populateRec(SVONode* node, uint8_t depth, glm::u8vec3 baseColor, glm::uvec3 initPos, glm::uvec3 endPos);
};
