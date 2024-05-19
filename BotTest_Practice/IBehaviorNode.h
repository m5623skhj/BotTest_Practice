#pragma once
#include <vector>
#include <memory>

#define DECLARE_SPTR(NodeType) using SPtr = std::shared_ptr<NodeType>;

enum class BehaviorStatus
{
	InvalidType = 0,
	Success,
	Failure,
	Running,
};
using NodeIdType = unsigned int;

class IBehaviorNode
{
public:
	DECLARE_SPTR(IBehaviorNode);

public:
	virtual ~IBehaviorNode() = default;

public:
	void AddChildNode(IBehaviorNode::SPtr childNode);
	virtual BehaviorStatus Do() = 0;

public:
	NodeIdType GetNodeId() { return nodeId; }
	void SetNodeId(NodeIdType inNodeId) { nodeId = inNodeId; }

	virtual bool CanAddChild() { return true; }

protected:
	std::vector<IBehaviorNode::SPtr> childrenNode{};
	NodeIdType nodeId{};
};
