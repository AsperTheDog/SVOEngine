#pragma once
#include <cstdint>
#include <vector>

class SVONode;

class NodeIndexer
{
public:
	explicit NodeIndexer(uint64_t maxIndexes);

	SVONode* getNode(uint32_t index, uint8_t offset = 0);
	uint32_t addNode();
	void seal();
	uint32_t getMaxIndexes();
	uint32_t size();

	static bool test();

private:
	std::vector<SVONode> data{};
	uint32_t maxIndexes;
};

struct ChildIndexer
{
	uint8_t index;

	void setChild(uint8_t pos, bool setValue)
	{
		if (setValue)
			index |= 1U << pos;
		else
			index &= ~(1U << pos);
	}

	bool getChild(uint8_t pos)
	{
		return (index >> pos) & 1U;
	}

	bool isEmpty()
	{
		return this->index == 0;
	}

	uint8_t getChildCount()
	{
		uint8_t count = 0;
		while (this->index) {
			count += this->index & 1;
			this->index >>= 1;
		}
		return count;
	}
};

