#include<unordered_map>
#include<string>
#include<map>

#include <iostream>

struct ListNode
{
	ListNode* prev = nullptr;
	ListNode* next = nullptr;
	ListNode* rand = nullptr;
	std::string data;
};

class List
{
public:
	void Serialize(FILE* file);
	void Deserialize(FILE* file);
private:
	std::unordered_map<const ListNode*, int> getNodesPositions() const;
	std::map<int, ListNode*> getNodesOfPositions();
	void setNodesRandPointers(std::map<ListNode*, int> randNodesPositions);
	void closeFileWithErrorMsg(FILE *file, std::string err);

	ListNode* head = nullptr;
	ListNode* tail = nullptr;
	int count = 0;
};

void List::Serialize(FILE* file4)
{
	if (head == nullptr)
		return;
	auto curNode = head;
	const short numToWrite = 1;
	if (fwrite(&count, sizeof(int), 1, file4) != 1)
	{
		std::cout << "Write data ERROR!\n";
		return;
	}
	auto nodesPositions = getNodesPositions();
	while (curNode)
	{
		int randElementPos = -1;
		if (curNode->rand)
			randElementPos = nodesPositions[curNode->rand];
		int curElementPos = nodesPositions[curNode];
		std::string data = curNode->data;
		size_t dataLen = data.length();
		if (fwrite(&curElementPos, sizeof(int), numToWrite, file4) != numToWrite)
			return closeFileWithErrorMsg(file4, "Write Error");
		if (fwrite(&randElementPos, sizeof(int), numToWrite, file4) != numToWrite)
			return closeFileWithErrorMsg(file4, "Write Error");
		if (fwrite(&dataLen, sizeof(size_t), numToWrite, file4) != numToWrite)
			return closeFileWithErrorMsg(file4, "Write Error");
		if (fwrite(data.c_str(), dataLen, numToWrite, file4) != numToWrite)
			return closeFileWithErrorMsg(file4, "Write Error");
		curNode = curNode->next;
	}
}

std::unordered_map<const ListNode*, int> List::getNodesPositions() const
{
	std::unordered_map<const ListNode*, int> nodesPositions;
	auto curNode = head;
	int num = 0;
	while (curNode)
	{
		nodesPositions[curNode] = num;
		num++;
		curNode = curNode->next;
	}
	return nodesPositions;
}

void List::Deserialize(FILE* file4)
{
	head = nullptr;
	tail = nullptr;
	ListNode *previous = nullptr;
	std::map<ListNode*, int> randNodesPositions;
	const short numToRead = 1;
	if (fread(&count, sizeof(int), 1, file4) != 1)
		return closeFileWithErrorMsg(file4, "Read Error");
	int c = count;
	while (c--)
	{
		auto node = new ListNode;
		int curNodePosition = 0;
		int curNodeRandPosition = 0;
		size_t dataLength = 0;
		if (fread(&curNodePosition, sizeof(int), numToRead, file4) != numToRead)
			return closeFileWithErrorMsg(file4, "Read Error");
		if (fread(&curNodeRandPosition, sizeof(int), numToRead, file4) != numToRead)
			return closeFileWithErrorMsg(file4, "Read Error");
		if (fread(&dataLength, sizeof(size_t), numToRead, file4) != numToRead)
			return closeFileWithErrorMsg(file4, "Read Error");
		std::string nodeData;
		nodeData.resize(dataLength);
		if (fread(&nodeData[0], sizeof(char), dataLength, file4) != dataLength)
			return closeFileWithErrorMsg(file4, "Read Error");
		node->data = nodeData;
		if (curNodeRandPosition != -1)
			randNodesPositions[node] = curNodeRandPosition;
		node->next = nullptr;
		node->prev = previous;
		if (previous)
			previous->next = node;

		if (!head)
			head = node;
		previous = node;
	}
	tail = previous;
	setNodesRandPointers(randNodesPositions);
}

void List::setNodesRandPointers(std::map<ListNode*, int> randNodesPositions)
{
	auto nodesPoitions = getNodesOfPositions();
	for (auto n : randNodesPositions)
		n.first->rand = nodesPoitions[n.second];
}

std::map<int, ListNode*> List::getNodesOfPositions()
{
	std::map<int, ListNode*> positions;
	int pos = 0;
	auto curNode = head;
	while (curNode)
	{
		positions[pos] = curNode;
		curNode = curNode->next;
		pos++;
	}
	return positions;
}

void List::closeFileWithErrorMsg(FILE *file, std::string err)
{
	std::cout << err << "\n";
	fclose(file);
}
