#include "PreCompile.h"
#include "IBehaviorNode.h"

void IBehaviorNode::AddChildNode(IBehaviorNode::SPtr childNode)
{
	childrenNode.emplace_back(childNode);
}