#include "PreCompile.h"
#include "Decorator.h"
#include <iostream>

DecoratorLoop::DecoratorLoop(unsigned short inMaxLoopCount, const bool inIsLoopingAllAtOnce, const bool inIsAutoReset)
	: maxLoopCount(inMaxLoopCount)
	, isLoopingAllAtOnce(inIsLoopingAllAtOnce)
	, isAutoReset(inIsAutoReset)
{
}

BehaviorStatus DecoratorLoop::Do()
{
	if (childrenNode.empty() == true)
	{
		return BehaviorStatus::InvalidType;
	}

	BehaviorStatus status;
	while(true)
	{
		for (auto& node : childrenNode)
		{
			status = node->Do();
			if (status != BehaviorStatus::Success)
			{
				return status;
			}
		}

		++loopCount;
		if (loopCount >= maxLoopCount)
		{
			if (isAutoReset == true)
			{
				loopCount = 0;
			}

			return BehaviorStatus::Success;
		}

		if (isLoopingAllAtOnce == false)
		{
			return BehaviorStatus::Running;
		}
	}
}

BehaviorStatus DecoratorInverter::Do()
{
	if (childrenNode.empty() == true)
	{
		return BehaviorStatus::InvalidType;
	}

	BehaviorStatus status;
	for (auto& node : childrenNode)
	{
		status = node->Do();
		if (status == BehaviorStatus::Failure)
		{
			continue;
		}
		else if (status == BehaviorStatus::Success)
		{
			return BehaviorStatus::Failure;
		}

		return status;
	}

	return BehaviorStatus::Success;
}

DecoratorDelay::DecoratorDelay(unsigned int inDurationMilisecond, bool inIsAutoReset)
	: durationMilisecond(inDurationMilisecond)
	, isAutoReset(inIsAutoReset)
{
}

BehaviorStatus DecoratorDelay::Do()
{
	const auto now = std::chrono::system_clock::now();
	if (startTime == std::chrono::system_clock::time_point())
	{
		startTime = now + std::chrono::milliseconds(durationMilisecond);
	}

	if (std::chrono::system_clock::now() < startTime)
	{
		return BehaviorStatus::Running;
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

	if (isAutoReset == true)
	{
		startTime = std::chrono::system_clock::time_point();
	}

	return BehaviorStatus::Success;
}

DecoratorRetry::DecoratorRetry(unsigned char inMaxRetryCount)
	: maxRetryCount(inMaxRetryCount)
{
}

BehaviorStatus DecoratorRetry::Do()
{
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

	++retryCount;
	if (retryCount < maxRetryCount)
	{
		return BehaviorStatus::Running;
	}

	return BehaviorStatus::Success;
}