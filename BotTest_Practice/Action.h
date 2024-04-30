#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "nlohmann/json.hpp"

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
	void Initialize();

public:
	void DoBotAction(Bot& targetBot);

private:
	bool ReadBotTestScenario();
	std::optional<nlohmann::json> OpenTestScenarioJson();
	bool MakeTestScenarioObject(const nlohmann::json& testScenarioJson);
	std::shared_ptr<IBotAction> MakeBotActionObject(std::string_view actionString);

private:
	std::vector<std::pair<std::string_view, std::shared_ptr<IBotAction>>> actionScenario;
	std::unordered_map<std::string_view, ActionFactoryFunction> actionFactoryMap;

	const std::string scenarioFilePath = "BotScenario.json";
};

#pragma region BotAction
struct IBotAction
{
	virtual void InitAction(const nlohmann::json& json) {}
	virtual void DoAction(Bot& targetBot) = 0;
};

struct BotAction_Ping : public IBotAction
{
	void DoAction(Bot& targetBot) override;
};

struct BotActionKeyword_LoopStart : public IBotAction
{
	void InitAction(const nlohmann::json& json);
	void DoAction(Bot& targetBot) override;
};

struct BotActionKeyword_LoopEnd : public IBotAction
{
	void InitAction(const nlohmann::json& json);
	void DoAction(Bot& target) override;
};

#define REGISTER_BOT_ACTION(ActionType, ActionString){\
	RegisterBotAction<ActionType>(ActionString);\
}

#define REGISTER_ALL_BOT_ACTION(){\
	REGISTER_BOT_ACTION(BotAction_Ping, Ping);\
}
#pragma endregion BotAction