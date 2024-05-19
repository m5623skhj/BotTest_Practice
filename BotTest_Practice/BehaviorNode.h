#pragma once
#include "IBehaviorNode.h"
#include <functional>

using BehaviorActionType = std::function<BehaviorStatus()>;

class BehaviorAction : public IBehaviorNode
{
public:
	DECLARE_SPTR(BehaviorAction);

public:
	BehaviorAction() = delete;
	explicit BehaviorAction(BehaviorActionType&& inAction);
	~BehaviorAction() override = default;

public:
	BehaviorStatus Do() override;
	bool CanAddChild() override { return false; }

private:
	BehaviorActionType action;
};

using BehaviorConditionType = std::function<bool()>;

class BehaviorCondition : public IBehaviorNode
{
public:
	DECLARE_SPTR(BehaviorCondition);

public:
	BehaviorCondition() = delete;
	explicit BehaviorCondition(BehaviorConditionType&& inCondition);
	~BehaviorCondition() override = default;

public:
	BehaviorStatus Do() override;

private:
	BehaviorConditionType condition;
};

class BehaviorSequence : public IBehaviorNode
{
public:
	DECLARE_SPTR(BehaviorSequence);

public:
	BehaviorSequence() = default;
	~BehaviorSequence() override = default;

public:
	BehaviorStatus Do() override;
};

class BehaviorSelector : public IBehaviorNode
{
public:
	DECLARE_SPTR(BehaviorSelector);

public:
	BehaviorSelector() = default;
	~BehaviorSelector() override = default;

public:
	BehaviorStatus Do() override;
};