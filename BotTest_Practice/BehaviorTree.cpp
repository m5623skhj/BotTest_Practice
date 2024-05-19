#include "PreCompile.h"
#include "BehaviorTree.h"
#include <iostream>

BehaviorStatus BehaviorTree::Tick()
{
	if (rootNode == nullptr)
	{
		return BehaviorStatus::InvalidType;
	}

	return rootNode->Do();
}

void BehaviorTree::SetTickDuration(const unsigned int inTickDurationMilisecond)
{
	tickDurationMilisecond = inTickDurationMilisecond;
}

bool BehaviorTree::AddRootNode(IBehaviorNode::SPtr node)
{
	return AddChildNode(node, 0, 0);
}

bool BehaviorTree::AddChildNode(IBehaviorNode::SPtr node, const NodeIdType targetParentNodeId, const NodeIdType myNodeId)
{
	if (node == nullptr)
	{
		PrintAddChildNodeFailed(targetParentNodeId, myNodeId);
		return false;
	}

	if (myNodeId == 0)
	{
		if (rootNode != nullptr)
		{
			PrintAddChildNodeFailed(0, myNodeId);
			return false;
		}

		rootNode = node;
		lastAddedNode = rootNode;
		nodeMap.insert({ 0, node });

		return true;
	}

	const auto& parentNode = nodeMap.find(targetParentNodeId);
	if (parentNode == nodeMap.end() ||
		parentNode->second == nullptr ||
		parentNode->second->CanAddChild() == false)
	{
		PrintAddChildNodeFailed(targetParentNodeId, myNodeId);
		return false;
	}

	if (nodeMap.insert({ myNodeId, node }).second == false)
	{
		PrintAddChildNodeFailed(targetParentNodeId, myNodeId);
		return false;
	}

	lastAddedNode = rootNode;
	parentNode->second->AddChildNode(node);

	return true;
}

void BehaviorTree::PrintAddChildNodeFailed(const NodeIdType parentNodeId, const NodeIdType childNodeId)
{
	std::cout << "AddChildNode() failed with " << parentNodeId
		<< " and " << childNodeId << std::endl;
}
