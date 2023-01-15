#include "SVO.hpp"

#include <iostream>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/string_cast.hpp>

#include "ColorFunctions.hpp"

glm::uvec3 offsets[] = {
	glm::uvec3(0, 0, 0),
	glm::uvec3(0, 0, 1),
	glm::uvec3(0, 1, 0),
	glm::uvec3(0, 1, 1),
	glm::uvec3(1, 0, 0),
	glm::uvec3(1, 0, 1),
	glm::uvec3(1, 1, 0),
	glm::uvec3(1, 1, 1),
};

bool intersectAABB(Ray* ray, glm::uvec3& min, glm::uvec3& max, float* t)
{
	float t1 = ((float)min.x - ray->origin.x) * ray->directionInv.x;
    float t2 = ((float)max.x - ray->origin.x) * ray->directionInv.x;
    float t3 = ((float)min.y - ray->origin.y) * ray->directionInv.y;
    float t4 = ((float)max.y - ray->origin.y) * ray->directionInv.y;
    float t5 = ((float)min.z - ray->origin.z) * ray->directionInv.z;
    float t6 = ((float)max.z - ray->origin.z) * ray->directionInv.z;
	
    float tmin = glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6));
    float tmax = glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6));
	
	float finalT = tmin <= 0 ? tmax : tmin;
	bool res = tmax >= tmin && finalT > 0;
	if (res) *t = finalT;

    return res;
}

void SVONode::setChildren(ChildIndexer indexes, uint32_t childPos)
{
	this->childrenLocator = indexes;
	this->childIndex = childPos;
}

void SVONode::setColor(glm::u8vec3 color)
{
	this->color = color;
}

bool SVONode::isChildAt(uint8_t pos)
{
	return this->childrenLocator.getChild(pos);
}

uint32_t SVONode::getChildrenIndex()
{
	return this->childIndex;
}

bool SVONode::intersectRay(Ray* ray, glm::uvec3 initPos, glm::uvec3 endPos, SVO* parent)
{
	float t;
	if (!intersectAABB(ray, initPos, endPos, &t)) return false;
	uint32_t length = (endPos.x - initPos.x);

	if (this->childrenLocator.isEmpty() || t > parent->LODLevel * length)
	{
		if (ray->t > t)
		{
			ray->color = this->color;
			ray->t = t;
		}
		return true;
	}

	length >>= 1;
	uint8_t childCount = 0;
	bool hit = false;
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (this->isChildAt(i))
		{
			glm::uvec3 newPos = initPos + (offsets[i] * length);
			if (parent->getChild(this->getChildrenIndex(), childCount)->intersectRay(ray, newPos, newPos + length, parent))
			{
				hit = true;
			}
			++childCount;
		}
	}
	return hit;
}

SVO::SVO(uint8_t maxDepth, uint64_t maxMemBytes, glm::u8vec3 (*getColor)(glm::u8vec3, glm::uvec3, glm::uvec3, uint8_t depth))
: LODLevel(5000U), indexer{maxMemBytes / sizeof(SVONode)}, maxDepth(maxDepth), getColor(getColor)
{
}
void SVO::populate(uint8_t sizeMult)
{
	this->root = SVONode();
	this->populateRec(&this->root, 0, glm::u8vec3(128, 128, 128), glm::uvec3(0), glm::uvec3(1 << (this->maxDepth + (sizeMult - 1))));
	//this->seal();
}

void SVO::seal()
{
	indexer.seal();
}

SVONode* SVO::getRoot()
{
	return &this->root;
}

SVONode* SVO::getChild(uint32_t index, uint8_t offset)
{
	return this->indexer.getNode(index, offset);
}

bool SVO::intersectRay(Ray* ray, uint8_t sizeMult)
{
	ray->directionInv = 1.f/ray->direction;
	return this->root.intersectRay(ray, glm::uvec3(0), glm::uvec3(1 << (this->maxDepth + (sizeMult - 1))), this);
}

void SVO::populateRec(SVONode* node, uint8_t depth, glm::u8vec3 baseColor, glm::uvec3 initPos, glm::uvec3 endPos)
{
	ChildIndexer children = {0};
	uint32_t childPos = this->indexer.size();
	uint64_t maxIndexes = this->indexer.getMaxIndexes();
	uint32_t length = (endPos.x - initPos.x) >> 1;
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (this->indexer.size() == maxIndexes || depth == this->maxDepth) break;

		uint8_t random = ColorFunctions::rand2() % 100;
		if (random < 80)
		{
			children.setChild(i, true);
			this->indexer.addNode();
		}
	}

	node->setColor(baseColor);
	node->setChildren(children, childPos);
	uint8_t childCount = 0;
	for (uint8_t i = 0; i < 8; ++i)
	{
		if (node->isChildAt(i))
		{
			glm::u8vec3 newColor = this->getColor(baseColor, initPos, endPos, depth);
			glm::uvec3 newPos = initPos + (offsets[i] * length);
			this->populateRec(
				this->indexer.getNode(node->getChildrenIndex(), childCount), 
				depth + 1, 
				newColor, 
				newPos, newPos + length);
			++childCount;
		}
	}
}
