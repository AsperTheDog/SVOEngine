#include "SVOAddons.hpp"

#include "SVO.hpp"
#include <iostream>

NodeIndexer::NodeIndexer(uint64_t maxIndexes)
{
	this->maxIndexes = maxIndexes;
	if (this->maxIndexes == 0) 
		return;

	data.reserve(this->maxIndexes + 1);
}

SVONode* NodeIndexer::getNode(uint32_t index, uint8_t offset)
{
	return &this->data.at(index + offset);
}

uint32_t NodeIndexer::addNode()
{
	if (data.size() >= this->maxIndexes) 
		throw std::runtime_error("Tried to add more nodes than allowed");

	this->data.emplace_back();

	return this->data.size() - 1;
}

void NodeIndexer::seal()
{
	data.shrink_to_fit();
}

uint32_t NodeIndexer::getMaxIndexes()
{
	return this->maxIndexes;
}

uint32_t NodeIndexer::size()
{
	return this->data.size();
}

bool NodeIndexer::test()
{
	std::vector<std::pair<SVONode*, uint32_t>> elems;
	NodeIndexer testing = NodeIndexer(100);

	for (int i = 0; i < 100; ++i)
	{
		uint32_t pos = testing.addNode();
		if (&testing.data.at(pos) != testing.getNode(pos)) return false;
		elems.emplace_back(testing.getNode(pos), pos);
	}

	if (testing.getMaxIndexes() != 100 || testing.size() != 100) return false;

	for (auto elem : elems)
	{
		if (testing.getNode(elem.second) != elem.first) return false;
	}

	try
	{
		testing.addNode();
	}
	catch (std::runtime_error& e)
	{
		return true;
	}
	return false;
}
