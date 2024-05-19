#pragma once
#include "BehaviorNode.h"
#include <unordered_map>

class BehaviorTree
{
public:
	BehaviorTree() = default;
	~BehaviorTree() = default;

public:
	BehaviorStatus Tick();
	void SetTickDuration(const unsigned int inTickDurationMilisecond);

public:
	bool AddRootNode(IBehaviorNode::SPtr node);
	bool AddChildNode(IBehaviorNode::SPtr node, const NodeIdType targetParentNodeId, const NodeIdType myNodeId);

public:
	size_t GetBehaviorTreeSize() { return nodeMap.size(); }

private:
	inline void PrintAddChildNodeFailed(const NodeIdType parentNodeId, const NodeIdType childNodeId);

private:
	IBehaviorNode::SPtr rootNode = nullptr;
	IBehaviorNode::SPtr lastAddedNode = nullptr;
	std::unordered_map<NodeIdType, IBehaviorNode::SPtr> nodeMap;

	NodeIdType nodeIdGenerator{};

	unsigned int tickDurationMilisecond{};
};