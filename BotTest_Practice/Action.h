#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>

struct IBotAction;
class Bot;

using ActionFactoryFunction = std::function<std::shared_ptr<IBotAction>()>;

class BotActionManager
{
private:
	BotActionManager() = default;
	~BotActionManager() = default;
	BotActionManager(const BotActionManager& other) = delete;
	BotActionManager& operator=(const BotActionManager& other) = delete;

public:
	BotActionManager& GetInst();

public:
	template <typename BotActionType>
	void RegisterBotAction(const std::string& key)
	{
		static_assert(std::is_base_of<IBotAction, BotActionType>::value, "RegisterBotAction must ingerit from IBotAction");
		ActionFactoryFunction func = []()
		{
			return std::make_shared<BotActionType>();
		};

		actionFactoryMap[key] = func;
	}

public:
	void DoBotAction(Bot& targetBot);

private:
	void Initialize();
	bool ReadBotTestScenario();

private:
	std::vector<std::string> actionScenario;
	std::unordered_map<std::string, ActionFactoryFunction> actionFactoryMap;

	const std::string scenarioFilePath = "BotScenario.json";
};

#pragma region BotAction
struct IBotAction
{
	virtual void DoAction(Bot& targetBot) = 0;
};

struct BotAction_Ping : public IBotAction
{
	void DoAction(Bot& targetBot) override;
};

#define REGISTER_BOT_ACTION(ActionType, ActionString){\
	RegisterBotAction<ActionType>(ActionString);\
}

#define REGISTER_ALL_BOT_ACTION(){\
	REGISTER_BOT_ACTION(BotAction_Ping, Ping);\
}
#pragma endregion BotAction