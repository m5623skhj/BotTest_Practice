#include "PreCompile.h"
#include "BehaviorNode.h"
#include <utility>

BehaviorAction::BehaviorAction(BehaviorActionType&& inAction)
	: action(std::move(inAction))
{
}

BehaviorStatus BehaviorAction::Do()
{
	if (action)
	{
		return action();
	}

	return BehaviorStatus::InvalidType;
}

BehaviorCondition::BehaviorCondition(BehaviorConditionType&& inCondition)
	: condition(std::move(inCondition))
{
}

BehaviorStatus BehaviorCondition::Do()
{
	if (condition() == false)
	{
		return BehaviorStatus::Failure;
	}

	BehaviorStatus status;
	for (auto& node : childrenNode)
	{
		status = node->Do();
		if (status == BehaviorStatus::Success)
		{
			continue;
		}

		return status;
	}
	return BehaviorStatus::Success;
}

BehaviorStatus BehaviorSequence::Do()
{
	if (childrenNode.empty() == true)
	{
		return BehaviorStatus::InvalidType;
	}

	BehaviorStatus status;
	for (auto& node : childrenNode)
	{
		status = node->Do();
		if (status == BehaviorStatus::Success)
		{
			continue;
		}

		return status;
	}

	return BehaviorStatus::Success;
}

BehaviorStatus BehaviorSelector::Do()
{
	if (childrenNode.empty() == true)
	{
		return BehaviorStatus::InvalidType;
	}

	BehaviorStatus status;
	for(auto& node : childrenNode)
	{
		status = node->Do();
		if (status != BehaviorStatus::Failure)
		{
			return status;
		}
	}

	return BehaviorStatus::Failure;
}