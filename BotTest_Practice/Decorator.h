#pragma once
#include "IBehaviorNode.h"
#include <chrono>

class IDecorator : public IBehaviorNode
{
public:
	bool CanAddChild() override { return childrenNode.empty(); }
};

class DecoratorLoop : public IBehaviorNode
{
public:
	DECLARE_SPTR(DecoratorLoop);

public:
	DecoratorLoop() = delete;
	explicit DecoratorLoop(unsigned short inMaxLoopCount, const bool inIsLoopingAllAtOnce, const bool inIsAutoReset = true);
	~DecoratorLoop() override = default;

public:
	BehaviorStatus Do() override;

private:
	const unsigned short maxLoopCount;
	unsigned short loopCount{};
	const bool isLoopingAllAtOnce{};
	const bool isAutoReset{};
};

class DecoratorInverter : public IBehaviorNode
{
public:
	DECLARE_SPTR(DecoratorInverter);

public:
	DecoratorInverter() = default;
	~DecoratorInverter() = default;

public:
	BehaviorStatus Do() override;
};

class DecoratorDelay : public IBehaviorNode
{
public:
	DECLARE_SPTR(DecoratorDelay);

public:
	DecoratorDelay() = delete;
	explicit DecoratorDelay(unsigned int inDurationMilisecond, bool inIsAutoReset = true);
	~DecoratorDelay() = default;

public:
	BehaviorStatus Do() override;

private:
	const unsigned int durationMilisecond{};
	std::chrono::system_clock::time_point startTime{};
	bool isAutoReset{};
};

class DecoratorRetry : public IBehaviorNode
{
public:
	DECLARE_SPTR(DecoratorRetry);

public:
	DecoratorRetry() = delete;
	explicit DecoratorRetry(unsigned char inMaxRetryCount);
	~DecoratorRetry() = default;

public:
	BehaviorStatus Do() override;

private:
	unsigned char maxRetryCount{};
	unsigned char retryCount{};
};